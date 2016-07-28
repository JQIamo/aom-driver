#include "LCD/LCD.h"
#include "Menu/MenuEncoder.h"
#include <EEPROM.h>
#include "aom.h"



//
// create LCD object
LCD_ST7032 lcd(LCD_RST, LCD_RS, LCD_CS);
// create encoder object
EncoderStream enc(LCDENC);

AD56XXR dac(DAC_SYNC, DAC_SCK, DAC_MOSI, 16, 5.0, 0.0, 1);
AOM aom(&dac);

typedef void (*callback_t)(char);

typedef struct {
  float val;
  //uint16_t dacVal;
  int incr;
  float increments[4];
  callback_t callback;
} MenuContext_t;


// forward declare these to avoid compiler complaints, and so callback can modify struct
void vco_context(char);
void att_context(char);
void set_max(char);

// menu construction
MenuContext_t vco = {.val = 0.200, .incr = 0, .increments = {1.000, 0.100, 0.010, 0.001}, .callback=vco_context};
MenuContext_t att = {.val = 1.05, .incr = 0, .increments = {1, 0.1, 0.01, 0.001}, .callback=att_context};
MenuContext_t max = {.val = 5.0, .incr = 0, .increments = {1, 0.1, 0.01, 0.001}, .callback=set_max};

MenuContext_t * active_context = &vco;


void set_max(char c){
  if (c == '+'){
    //vco.val += vco.increments[vco.incr % 4];
    max.val = constrain(max.val + max.increments[max.incr %4], 0.0, 5.0);
  } else {
    //vco.val-= vco.increments[vco.incr %4];
    max.val = constrain(max.val - max.increments[max.incr % 4], 0.0, 5.0);
  }
}
// char c: passed form MenuEncoder stream
void vco_context(char c){
  if (c == '+'){
    //vco.val += vco.increments[vco.incr % 4];
    vco.val = constrain(vco.val + vco.increments[vco.incr %4], 0.0, 5.0);
  } else {
    //vco.val-= vco.increments[vco.incr %4];
    vco.val = constrain(vco.val - vco.increments[vco.incr % 4], 0.0, 5.0);
  }
    dac.setVolt(1, vco.val);
  // update DAC here
}

void att_context(char c){
  if (c == '+'){
    att.val = constrain(att.val + att.increments[att.incr %4], 0.0, max.val);
  } else {
    att.val = constrain(att.val - att.increments[att.incr % 4], 0.0, max.val);
  }
  dac.setVolt(0, att.val);
}

void menu_incr(MenuContext_t * ctx, char c){
  switch(c){
    case '+':
      //ctx->val += ctx->increments[ctx->incr % 4];
      // pass up the ladder...
      ctx->callback(c);
      break;
    case '-':
      ctx->callback(c);
      break;
    case 'H':
      if (ctx == &vco){
        active_context = &att;
      } else if (ctx == &att){
        active_context = &vco;
      } else {
        active_context = &vco;
        redraw_menu();
        return;
      }
      ctx = active_context;
      redraw_carat();
      break;
    case 'P':
      ctx->incr = (ctx->incr + 1) % 4;
      break;
    case 'C':
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.write("Cheat activated!");
      delay(1000);
      active_context = &max;
      draw_settings();
      return;
  }
  redraw_voltage(ctx);

}

void draw_settings(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.write("MAX ATTEN:");
  lcd.setCursor(0, 1);
  lcd.write(">");
  redraw_voltage(&max);

}


void redraw_carat(){
  lcd.setCursor(0,0);
  if (active_context == &vco){
    lcd.print(">");
  } else {
    lcd.print(" ");
  }
  lcd.setCursor(0, 1);
  if (active_context == &att){
    lcd.print(">");
  } else {
    lcd.print(" ");
  }
}


void redraw_voltage(MenuContext_t * ctx){
  int cursor_row;
  if (ctx == &vco){
    lcd.setCursor(6, 0);
    cursor_row = 0;
  } else{
    lcd.setCursor(6,1);
    cursor_row = 1;
  }

  char buff[8];
  Serial.printf("%-5.3f\n", ctx->val);

//   //dtostrf(ctx->val, -5, 3, buff);
//   Serial.write(buff);
// //  buff[6] = '\0';
//   Serial.println("asdf");
//   Serial.write(buff);
//   Serial.println("");
  lcd.printf("%-5.3f", ctx->val);
//  Serial.println(ctx->val);

  int cursor_offset;
  if (active_context->incr == 0){
    cursor_offset = 0;
  }else {
    cursor_offset = active_context->incr + 1;
  }
  lcd.setCursor(6+cursor_offset, cursor_row );
}


void redraw_menu(){
  lcd.clear();
  lcd.setCursor(0,0);
  if (active_context == &vco){
    lcd.print(">");
  } else {
    lcd.print(" ");
  }
  lcd.print("VCO:       ");
  //lcd.print("0.000 ");// fill in with value
  lcd.print("V   ");

  lcd.setCursor(0, 1);
  if (active_context == &att){
    lcd.print(">");
  } else {
    lcd.print(" ");
  }
  lcd.print("ATT:       ");
  //lcd.print("0.000 ");// fill in with value
  lcd.print("V   ");

  redraw_voltage(&vco);
  redraw_voltage(&att);

  if (active_context == &vco){
    lcd.setCursor(6,0);
  } else {
    lcd.setCursor(6,1);
  }
}




// void draw_att(){
//   lcd.setCursor()
// }




void setup() {



  Serial.begin(9600);
  SPI.begin();

  aom.init();

  // initialize DAC, etc. to default value
  dac.setIntRef(1);
  dac.setVolt(0, 0.0);
  dac.setVolt(1, 0.0);
  //init encoder
  enc.begin();

  // init LCD
  lcd.begin();
  lcd.clear();
  lcd.cursor();
  //lcd.blink();
  lcd.print("Starting...");
  //lcd.moveCursorLeft();
  lcd.setCursor(3, 0);
  //delay(3000);
  redraw_menu();
}



void loop() {
  if (enc.available() > 0){
    char c = enc.read();
    Serial.println(c);
    //active_context->callback(c);
    menu_incr(active_context, c);
  }
}
