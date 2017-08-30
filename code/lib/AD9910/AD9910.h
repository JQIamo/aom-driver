/*
   AD9910.h - AD9910 DDS communication library
   Based on AD9914 by Ben Reschovsky, 2016.
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
   aunsigned long with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

//// THIS NEEDS TO BE UPDATED!! USING OLD VERSION FOR TESTING

#ifndef AD9910_h
#define AD9910_h

#include "Arduino.h"

#define CLOCKSPEED 1000000

class AD9910
{
    typedef union {
        uint8_t bytes[8] = {0};
        uint32_t block[2];
    } data_t;

    typedef struct {
        data_t data;
        uint8_t addr;
        uint8_t bytes = 4;  // number of bytes to write
    } reg_t;




    public:
        // Constructor function.
        AD9910(int ssPin, int resetPin, int updatePin, int ps0, int ps1, int ps2, int osk);


        // Initialize with refIn frequency, and clock multiplier value
        void begin(unsigned long ref, uint8_t mult);

        // Reset the DDS
        void reset();

        // Update to load newly written settings
        void update();

        // Gets current frequency
        unsigned long getFreq(uint8_t profile = 0);

        // Sets frequency
        void setFreq(unsigned long freq, uint8_t profile = 0);

        // Gets current frequency tuning word
        unsigned long getFTW(uint8_t profile = 0);


        // Sets frequency tuning word
        void setFTW(unsigned long, uint8_t profile = 0);

/*    *********************** to implement later ***************
        //write scaled amplitude for the selected profile
        void setAmp(double scaledAmp, uint8_t profile = 0);

        void setAmpdB(double scaledAmpdB, uint8_t profile = 0);


        // Gets current amplitude
        double getAmp(uint8_t profile = 0);


        // Gets current amplitude in dB
        double getAmpdB(uint8_t profile = 0);


        // Gets current amplitude tuning word
        unsigned long getASF(uint8_t profile = 0);



        // places DDS in linear sweep mode
        //void linearSweep(unsigned long, unsigned long, unsigned long, byte, unsigned long, byte);

        //enable profile mode
        void enableProfileMode();

        //disable profile mode
        void disableProfileMode();

        //enable OSK
        void enableOSK();

        //disable OSK
        void disableOSK();

        //Get profile mode status
        bool getProfileSelectMode();

        //Get OSK mode status
        bool getOSKMode();

        //enable the Sync Clck output
        void enableSyncClck();

        //disable the Sync Clck output
        void disableSyncClck();

        //Change active profile mode:
        void selectProfile(byte);

        //Get currently active profile
        uint8_t getProfile();


 */
        void writeRegister(reg_t payload);

    private:
        // Instance variables that hold pinout mapping
        // from arduino to DDS pins.
        int _ssPin, _resetPin, _updatePin, _ps0, _ps1, _ps2, _osk;

        // Instance variables for frequency _freq, frequency tuning word _ftw,
        // reference clock frequency _refClk, etc.
        unsigned long _freq[8], _ftw[8], _refClk, _asf[8];
        double _scaledAmp[8], _scaledAmpdB[8];

        uint8_t _activeProfile;

        // Instance variables to keep track of the DDS mode:
        bool _profileModeOn, _OSKon;


        // write amplitude tuning word to device
        //void writeAmp(long ampScaleFactor, uint8_t profile);

        // DDS frequency resolution
         double RESOLUTION;// = 4294967296; // sets resolution to 2^32 = 32 bits. Using type double to avoid confusion with integer division...

};

#endif
