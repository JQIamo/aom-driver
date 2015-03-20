/*
   AOMDriver.h - Pin mappings for Teensy board attached to AOMDriver
   Created by Neal Pisenti, 2015
   JQI - Strontium - UMD

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

 */
 






// generic switches
#define SW1         1
#define SW2         27
#define SW3         28

// encoder
#define ENC_SW      0
#define ENC_A       9
#define ENC_B       23

// enable or disable VCO (ie, VCO powerdown)
#define VCO_EN      8

// main SPI; for DAC and display communication
#define SPI_MOSI    11
#define SPI_MISO    12
#define SPI_CLK     13

// DAC specific IO pins
#define DAC_CLR     14
#define DAC_LDAC    15
#define DAC_RST     16
#define DAC_SYNC    17

// LCD Display specific pins; use JQI's LCD library
#define LCD_RST     20
#define LCD_RS      21
#define LCD_CS      22

// attenuator control pins
#define ATTEN_LE    24
#define ATTEN_CLK   25
#define ATTEN_MOSI  26

// generic I2C bus
#define I2C_SDA     18
#define I2C_SCL     19

// Internal/External controls; source select/enable/disable
#define INT_EXT_RF_CTL      2  // RF source set from teensy or external BNC?
#define INT_EXT_OUTPUT_CTL  4  // output on/off set from teensy or external BNC?
#define EXT_DITHER_CTL      10 // external dither enabled or disabled
#define SERVO_CTL           6  // power feedback, or daughter board feedback?   
#define SETPT_CTL           7  // external servo setpoint, or dac setpoint?

#define TEENSY_RF_TTL       3  // RF source select
#define TEENSY_OUTPUT_TTL   5  // Output on/off


// analog power measurement pin
// will probably want a calibration function somewhere?
#define PWR_MEAS    A10
