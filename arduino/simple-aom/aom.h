#include "Arduino.h"
#include "AD56XXR/AD56XXR.h"


#define DAC_CLR 0
#define DAC_LDAC 1
#define DAC_SYNC 2
#define DAC_SCK 3
#define DAC_MOSI 4

#define AUX_TRIGGER 5

#define DDS_RESET 10
#define DDS_CS 12
#define DDS_OSK 14
#define DDS_PS0 15
#define DDS_PS1 16
#define DDS_IOUPDATE 17


#define ENC_SW 21
#define ENC_A 22
#define ENC_B 23


#define LCD_RST 20
#define LCD_RS 18
#define LCD_CS 19


// magic word for factory default
#define FACTORY_DEFAULT_EEPROM 314


struct AOMState {
  unsigned long dac0;
  unsigned long dac1;
  int factory_default;
} checkpoint;


typedef struct  {
  // transfer function parameters
  double a;
  double b;
  double c;
  double d;

  int address;
  bool valid;
} AOMCalibration_t;


class AOM {
public:
  AOM(AD56XXR * dac);
  void init();

  // frequency setpoint DAC word
  uint16_t get_dac_f();
  void set_dac_f(uint16_t dacVal);

  // attenuation DAC
  uint16_t get_dac_a();
  void set_dac_a(uint16_t dacVal);


  uint16_t freq_to_dac(double freq);
  double dac_to_freq(uint16_t dacVal);

  uint16_t pow_to_dac(double power);
  double dac_to_pow(uint16_t dacVal);
  float attinit, vcoinit;
  int eepromvco_ptr;

private:
  // whether to display voltage or calibration units
  bool _f_calib, _a_calib;
  AD56XXR * _dac;
  AOMCalibration_t _pow_calib;
  AOMCalibration_t _freq_calib;

  uint16_t _dac_a, _dac_f;
};

AOM::AOM(AD56XXR * dac):
  _dac(dac),
  _f_calib(0),
  _a_calib(0)
{
}

// init; pull in EEPROM data
void AOM::init(){

  pinMode(DAC_CLR, OUTPUT);
  digitalWrite(DAC_CLR, HIGH);

  pinMode(DAC_LDAC, OUTPUT);
  digitalWrite(DAC_LDAC, LOW);

  // pull in old AOM state checkpoint; program default values if necessary
  //EEPROM.get(0, checkpoint);
 // if(checkpoint.factory_default != FACTORY_DEFAULT_EEPROM){
 //   Serial.println("Factory default EEPROM detected; reprogramming");
  //  checkpoint.factory_default = FACTORY_DEFAULT_EEPROM;
//checkpoint.dac0 = 0;
 //   checkpoint.dac1 = 0;
 //   EEPROM.put(0, checkpoint);
 // }

  // increment to find calibration EEPROM address;
  // bootstraps for startup
 // int eeprom_ptr = sizeof(checkpoint);

  //EEPROM.get(eeprom_ptr, _freq_calib);
  //if (_freq_calib.address != eeprom_ptr){
  //  Serial.println("Factory default frequency calibration");
  //  _freq_calib.address = eeprom_ptr;
  //  _freq_calib.valid = false;
  //  _freq_calib.a = 0.0;
  //  _freq_calib.b = 0.0;
  //  _freq_calib.c = 0.0;
  //  _freq_calib.d = 0.0;
  //  EEPROM.put(eeprom_ptr, _freq_calib);
  //}

  // increment and get power calibration
  //eeprom_ptr += sizeof(_freq_calib);
  //EEPROM.get(eeprom_ptr, _pow_calib);
  //if (_pow_calib.address != eeprom_ptr){
  //  Serial.println("Factory default power calibration");
  //  _pow_calib.address = eeprom_ptr;
  //  _pow_calib.valid = false;
  //  _pow_calib.a = 0.0;
  //  _pow_calib.b = 0.0;
  //  _pow_calib.c = 0.0;
  //  _pow_calib.d = 0.0;
  //  EEPROM.put(eeprom_ptr, _pow_calib);
  //}
  
  EEPROM.get(0,attinit);
  eepromvco_ptr = sizeof(attinit);
  EEPROM.get(eepromvco_ptr,vcoinit);
  if(attinit != attinit){
    attinit = 0;
  }
  if(vcoinit != vcoinit){
    vcoinit = 0;
  }

  delay(1);


}
