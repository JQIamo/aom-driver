#include <SPI.h>

#include "LCD.h"
#include "Menu.h"
#include "EncoderStream.h"
#include "CSRManager.h"
#include "Thread.h"

#define DAC_CS        2
#define DAC_CLK       3
#define DAC_MOSI      4
#define DAC_CLR       7
#define DAC_LDAC      14

// pin defines; note some of these features are not yet implemented in library.
#define DDS_DROVER    0 // input
#define DDS_DRCTL     1
#define DDS_PS1       5
#define DDS_IOUPDATE  9
#define DDS_OSK       12
#define DDS_RESET     15
#define DDS_PLL_LOCK  18  // input
#define DDS_PS0       20
#define DDS_PS2       25
#define DDS_CS        28
#define DDS_DRHOLD    32

#define AUX_TRIG      10  // input

#define LCD_CS        16
#define LCD_RS        17
#define LCD_RST       19

#include "src/aom_menus.h"

const char * gitversion = GITVERSION;

uint16_t vcoVal = 32768;  // mid-scale
uint16_t attVal = 52428;  // = 4V = full attenuation
uint16_t attMin = 0;      // = 0V = max power (min attenuation)
uint32_t ddsFreq = 80000000;  // = 80MHz

LCD_ST7032 lcd(LCD_RST, LCD_RS, LCD_CS);
AD56XXR dac(DAC_CS, DAC_CLR, DAC_LDAC, DAC_CLK, DAC_MOSI);
AD9910 dds(DDS_CS, DDS_RESET, DDS_IOUPDATE, DDS_PS0, DDS_PS1, DDS_PS2, DDS_OSK);

using namespace EncoderStream;

int const EncoderStream::ENC_A = 22;
int const EncoderStream::ENC_B = 23;
int const EncoderStream::ENC_SW = 21;

Thread saveThread = Thread();

void saveToEEPROM(){
  csr.save(VCO_VAL);
  csr.save(ATT_VAL);
  csr.save(DDS_FREQ);
}


//#define DEBUG // uncomment this line if you want serial debugging turned on.

void setup(){


  csr.associate(VCO_VAL, &vcoVal);
  csr.associate(ATT_VAL, &attVal);
  csr.associate(ATT_MIN, &attMin);
  csr.associate(DDS_FREQ, &ddsFreq);

  csr.init_from_eeprom();

  saveThread.onRun(saveToEEPROM);
  saveThread.setInterval(60000);  // save checkpoint every minute

Serial.begin(9600);

#ifdef DEBUG
  while (!Serial){ ; }
  Serial.println("alive!!");
  delay(100);
#endif

  SPI.begin();
  delay(100);

  dds.begin(40000000, 25);
  dds.setFreq(ddsFreq);

  // attMin, attMax, vcoMin, vcoMax
  dac.begin(attMin, 52428, 0, 65535);
  dac.setVal(attVal, 0);
  dac.setVal(vcoVal, 1);

  Encoder.begin();

  lcd.begin();
  lcd.cursor();

  root.add(&att_menu);
  root.add(&vco_menu);

  // TODO: check if DDS installed by reading back from register
  // only enable menu if DDS available.
  root.add(&dds_menu);

  root.add(&att_min_menu);

  misc.add(&reset_dds_menu);
  misc.add(&reset_dac_menu);
  misc.add(&code_vers);

  code_vers.add(&code_version_menu);

  root.add(&misc);

  root.enter();

}


void loop(){

  if (Encoder.available() > 0){
    char c = Encoder.read();
    root._active->process(c);
  }

  if (saveThread.shouldRun()){
    saveThread.run();
  }

  // SetListArduino could be added here as well too.

}
