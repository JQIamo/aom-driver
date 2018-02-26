#include "AD56XXR.h"

extern uint16_t attVal;

// constructor for bitbang SPI
AD56XXR::AD56XXR(int cs, int clr, int ldac, int clk, int mosi){
  _cs = cs;
  _clr = clr;
  _ldac = ldac;
  _clk = clk;
  _mosi = mosi;
  max[0] = 65535;
  max[1] = 65535;
  min[0] = 0;
  min[1] = 0;

  _spi = 0;
}

// constructor for hardware SPI
AD56XXR::AD56XXR(int cs, int clr, int ldac){
  _cs = cs;
  _clr = clr;
  _ldac = ldac;

  _spi = 1;
  _spi_settings = SPISettings(AD56XXR_SPISPEED, MSBFIRST, SPI_MODE2);
}

void AD56XXR::begin(uint16_t min0, uint16_t max0, uint16_t min1, uint16_t max1){

  pinMode(_cs, OUTPUT);
  digitalWriteFast(_cs, HIGH);

  pinMode(_clr, OUTPUT);
  digitalWriteFast(_clr, HIGH);

  pinMode(_ldac, OUTPUT);
  digitalWriteFast(_ldac, LOW);

  // make sure you set up bitbang SPI pins if they're used
  if (_spi == 0){
    pinMode(_clk, OUTPUT);
    pinMode(_mosi, OUTPUT);
    digitalWriteFast(_clk, HIGH);
    digitalWriteFast(_mosi, LOW);
  }

  delay(1);
  // set internal vref
  uint32_t d = (AD56XXR_INT_REF_SETUP | 1);
  transfer(d);

  max[0] = max0;
  max[1] = max1;
  min[0] = min0;
  min[1] = min1;
}

// ch 0 is attenuator, ch 1 is vco
uint16_t AD56XXR::setVal(uint16_t val, int ch){
  uint16_t v;

  v = constrain(val, min[ch], max[ch]);

  uint32_t d = AD56XXR_WRITE_UPDATE_DAC_N | ((uint32_t)ch << 16) | v;
  transfer(d);
  _val[ch] = v;

  return v;
}

double AD56XXR::setVolt(double volt, int ch){

  uint16_t val = volt_to_val(volt);
  val = setVal(val, ch);

  return val_to_volt(val);
}


double AD56XXR::val_to_volt(uint16_t val){
  return (5.0*val/65535.0);
}

uint16_t AD56XXR::volt_to_val(double volt){
  return (uint16_t)(volt*65535/5.0);
}

void AD56XXR::transfer(uint32_t data){

  if (_spi){
    digitalWriteFast(_cs, LOW);
    SPI.beginTransaction(_spi_settings);
    SPI.transfer(((data >> 16) & 0xFF));
    SPI.transfer(((data >> 8) & 0xFF));
    SPI.transfer((data & 0xFF));
    SPI.endTransaction();
    digitalWriteFast(_cs, HIGH);
  } else {
    int send_bit;
    digitalWriteFast(_clk, HIGH);
    digitalWriteFast(_mosi, LOW);

    digitalWriteFast(_cs, LOW);

    for (int i = 23; i >= 0; i--){
      send_bit = (data >> i) & 0x01;

      digitalWriteFast(_mosi, send_bit);
      delayMicroseconds(1);

      digitalWriteFast(_clk, LOW);
      delayMicroseconds(1);
      digitalWriteFast(_clk, HIGH);
      delayMicroseconds(1);
    }

    digitalWriteFast(_cs, HIGH);
  }

}
