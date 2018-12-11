#ifndef AOM_MENUS_H
#define AOM_MENUS_H

#include "Menu.h"
#include "LCD.h"
#include "AD56XXR.h"
#include "AD9910.h"
#include "CSRManager.h"

extern LCD_ST7032 lcd;
extern AD56XXR dac;
extern AD9910 dds;
//extern CSRManager csr;

enum { VCO_VAL, ATT_VAL, ATT_MIN, DDS_FREQ, REG_COUNT};
CSRManager csr(REG_COUNT);

extern uint16_t vcoVal;
extern uint16_t attVal;
extern uint16_t attMin;
extern uint32_t ddsFreq;
extern const char * gitversion;

// #ifndef GITVERSION
//   #define GITVERSION "UNKNOWN"
// #endif

/******** VCO Menu ************/

void enter_vco(double & volt){
  volt = dac.val_to_volt(vcoVal);
}

void update_vco(double & volt){
  // which channel?
  volt = dac.setVolt(volt, 1);
  vcoVal = dac.volt_to_val(volt);
}

/******** Att Menu ************/

void enter_att(double & volt){

  volt = dac.val_to_volt(attVal);

}

void update_att(double & volt){

  volt = dac.setVolt(volt, 0);
  attVal = dac.volt_to_val(volt);

}


void enter_att_min(double & volt){
  volt = dac.val_to_volt(attMin);
}

void update_att_min(double & volt){
  // which channel?
  uint16_t val = dac.volt_to_val(volt);
  val = constrain(val, 0, dac.max[0]); // remember, 4V = full attenuation
  attMin = val;
  dac.min[0] = val; // update on DAC too.. redundant but lazy.
  volt = dac.val_to_volt(val);
}

void exit_att_min(double & volt){
  // which channel?
  //volt = dac.setVolt(volt, 1);
  csr.save(ATT_MIN);

}
/******** DDS Menu ************/

void enter_dds(double & freq){
  // freq in MHz, dds operates in Hz
  freq = (double)ddsFreq/1.0e6;
}

void update_dds(double & freq){

  freq = constrain(freq, 0.0, 400.0);
  // freq in MHz, dds operates in Hz
  ddsFreq = (uint32_t)(freq*1.0e6);

  dds.setFreq(ddsFreq);
  // this could likely be bundled with above line;
  // leaving for now to double check float <-> int conversions
  // aren't dropping significant digits.
  freq = (double)ddsFreq/1.0e6;
}

void reset_dds(int & v){
  // this should be refactored
  dds.begin(40000000, 25);
  dds.setFreq(ddsFreq);
}

void reset_dac(int & v){
  // this should be refactored
  dac.begin();
  // attMin, attMax, vcoMin, vcoMax
  dac.begin(attMin, 52428, 0, 65535);
  dac.setVal(attVal, 0);
  dac.setVal(vcoVal, 1);
}

void code_version(int & v){

}
/******** Menu Def ************/

double volt_incr[4] = {1.0, 0.1, 0.01, 0.001};
uint8_t volt_cursor[4] = {0, 2, 3, 4};

double freq_incr[6] = {10.0, 1.0, 0.1, 0.01, 0.001, 0.0001};
uint8_t freq_cursor[6] = {1, 2, 4, 5, 6, 7};

MenuNumeric<double> vco_menu = MenuNumeric<double>::create("VCO Voltage     ", &lcd)
    .line0("VCO:            ")
    .line1("               V")
    .incr(volt_incr, 4, 1)
    .cursors(volt_cursor)
    .formatter("%05.3f", 0, 1)
    .update_cb(update_vco)
    .enter_cb(enter_vco);


MenuNumeric<double> dds_menu = MenuNumeric<double>::create("DDS freq.       ", &lcd)
    .line0("dds:            ")
    .line1("             MHz")
    .incr(freq_incr, 6, 1)
    .cursors(freq_cursor)
    .formatter("%08.4f", 0, 1)
    .update_cb(update_dds)
    .enter_cb(enter_dds);

MenuNumeric<double> att_menu = MenuNumeric<double>::create("Atten. Voltage  ", &lcd)
    .line0("Att:            ")
    .line1("               V")
    .incr(volt_incr, 4, 1)
    .cursors(volt_cursor)
    .formatter("%05.3f", 0, 1)
    .update_cb(update_att)
    .enter_cb(enter_att);

MenuNumeric<double> att_min_menu = MenuNumeric<double>::create("Atten. Min       ", &lcd)
    .line0("Att:            ")
    .line1("               V")
    .incr(volt_incr, 4, 1)
    .cursors(volt_cursor)
    .formatter("%05.3f", 0, 1)
    .update_cb(update_att_min)
    .enter_cb(enter_att_min)
    .exit_cb(exit_att_min);

MenuSelection reset_dds_menu("Reset DDS       ", &lcd, reset_dds);
MenuSelection reset_dac_menu("Reset DAC       ", &lcd, reset_dac);
MenuSelection code_version_menu(gitversion, &lcd, code_version);

// should make a string menu
MenuController code_vers("Code vers >     ", &lcd);



MenuController misc("Misc >          ", &lcd);
MenuController root("Menu >          ", &lcd);

#endif
