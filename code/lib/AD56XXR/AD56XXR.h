#ifndef AD56XXR_H
#define AD56XXR_H

#include <Arduino.h>
#include <SPI.h>

#define AD56XXR_WRITE_INPUT_REG_N         0x00000000
#define AD56XXR_UPDATE_DAC_REG_N          0x00080000
#define AD56XXR_WRITE_INPUT_REG_N_UPDATE  0x00100000
#define AD56XXR_WRITE_UPDATE_DAC_N        0x00180000
#define AD56XXR_DAC_CH(N)                 ((uint32_t)N << 16)
#define AD56XXR_POWERDOWN                 0x00200000
#define AD56XXR_RESET                     0x00280000
#define AD56XXR_LDAC_REG_SETUP            0x00300000
#define AD56XXR_INT_REF_SETUP             0x00380000
#define AD56XXR_DAC_A                     0x00000000
#define AD56XXR_DAC_B                     0x00010000
#define AD56XXR_DAC_ALL                   0x00070000

#define AD56XXR_SPISPEED                  1000000


class AD56XXR {
public:
  AD56XXR(int cs, int clr, int ldac, int clk, int mosi);
  AD56XXR(int cs, int clr, int ldac);

  void begin( uint16_t min0 = 0, uint16_t max0 = 65535,
              uint16_t min1 = 0, uint16_t max1 = 65535);

  uint16_t max[2];
  uint16_t min[2];

  uint16_t volt_to_val(double volt);
  double val_to_volt(uint16_t val);

  double setVolt(double volt, int ch);
  uint16_t setVal(uint16_t val, int ch);

  inline double getVolt(int ch){
    return val_to_volt(getVal(ch));
  }

  inline uint16_t getVal(int ch){
    return _val[ch];
  }



  void transfer(uint32_t data);


private:
  int _cs, _clr, _ldac, _clk, _mosi, _spi;
  uint16_t _val[2];
  SPISettings _spi_settings;
};

#endif
