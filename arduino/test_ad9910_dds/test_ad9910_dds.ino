#include <SPI.h>
#include "AD9910.h"
#include "SetListArduino.h"


#define RESET 0

#define D1_CS         1
#define D1_OSK        2
#define D1_PS0        3
#define D1_PS1        4
#define D1_IOUPDATE   5
#define D1_PS2        6
#define D1_DRHOLD     7
#define D1_DRCTL      8
#define D1_DROVER     9
#define D1_PLL_LOCK   10

#define D2_CS         12
#define D2_OSK        15
#define D2_PS0        16
#define D2_PS1        17
#define D2_IOUPDATE   18
#define D2_PS2        19
#define D2_DRHOLD     20
#define D2_DRCTL      21
#define D2_DROVER     22
#define D2_PLL_LOCK   23


// CAREFUL!! reset is a global reset :/ dumb me.
AD9910 ddsAOM(D1_CS, RESET, D1_IOUPDATE, D1_PS0, D1_PS1, D1_PS2, D1_OSK);
AD9910 ddsBN(D2_CS, RESET, D2_IOUPDATE, D2_PS0, D2_PS1, D2_PS2, D2_OSK);

#define SETLIST_TRIG 32
SetListArduino SetListImage(SETLIST_TRIG)

void setFreq(AD9910 * dds, int * params){
    dds->setFreq(params[0]);
}

void setup() {
    Serial.begin(115200);
    SPI.begin();
        
    ddsAOM.initialize(40000000, 25);
    ddsBN.initialize(40000000, 25, 1);

    // some random defaults...
    ddsAOM.setFreq(100000000);
    ddsBN.setFreq(25000000);

    SetListImage.registerDevice(ddsAOM, 0);
    setListImage.registerDevice(ddsBN, 1);

    SetListImage.registerCommand("f", 0, setFreq);
    SetListImage.registerCommand("f", 1, setFreq);
    
}

void loop() {

  SetListImage.readSerial();
  // put your main code here, to run repeatedly:

}
