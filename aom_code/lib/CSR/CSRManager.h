#ifndef CSR_MANAGER_H
#define CSR_MANAGER_H


#include <EEPROM.h>

#define EEPROM_INIT_CHAR 0xBE

class CSRManager {
public:
  CSRManager(int regCount){
    csr = new void * [regCount];
    csr_addr = new int[regCount];
    csr_size = new int[regCount];

    csr_associated = new int[regCount];

    _eeprom_counter = 2;  // byte[0..1] is EEPROM initialize check
    _reg_counter = 0; // counts number of associated CSRs
  };



  //! csr: array of void pointers to (yet unspecified) SRAM variables.
  void ** csr;

  //! csr_addr: array of start addresses in EEPROM
  int * csr_addr;


  //! csr_size: array of CSR sizes, to avoid overwriting adjacent memory by accident.

  // storing this separately as extra precaution we don't overwrite
  // adjacent memory if write larger datatype in runtime.
  // ie, won't use sizeof() in runtime code, but push it to .associate(..)
  int * csr_size;

  // pointer array to keep track of registers associated. for loading eeprom, etc.
  // replaces _reg_counter
  int * csr_associated;

  //! associate a CSR with a (global scope) variable.
  template<class T> void associate(int reg, T * var){
    csr[reg] = static_cast<void*>(var);

    csr_addr[reg] = _eeprom_counter;
    _eeprom_counter += sizeof(T);
    _eeprom_counter += (_eeprom_counter % 2);  // keep aligned to 2-byte boundary
                                              // for less wear
    csr_size[reg] = sizeof(T);

    csr_associated[_reg_counter++] = reg;
  }


  void init_from_eeprom(){
    char init = EEPROM.read(0);
    if (init == EEPROM_INIT_CHAR){
      // Serial.println("init from eeprom underway");
      for (int i = 0; i < _reg_counter; i++){
        load(csr_associated[i]);
      }
    } else {
      // Serial.println("eeprom was not initialized");
      for (int i = 0; i < _reg_counter; i++){
        save(csr_associated[i]);
      }
      EEPROM.write(0, EEPROM_INIT_CHAR);
    }
  }


  // these interact with EEPROM
  void save(int reg){
    // from EEPROMAnything
    // https://github.com/collin80/EEPROMAnything
    // get byte pointer
    const byte * p = (const byte *) csr[reg];
    int ee = csr_addr[reg]; // get first address
    for (int i = 0; i < csr_size[reg]; i++){
      EEPROM.update(ee++, *p++);
    }
  }

  void load(int reg){
    // get byte pointer to object
    byte * p = (byte *) csr[reg];
    int ee = csr_addr[reg];
    for (int i = 0; i < csr_size[reg]; i++){
      *p++ = EEPROM.read(ee++);
    }
  }


  // don't shoot yourself in the foot!
  // know what datatype you're reading out, otherwise it'll be
  // garbage.
  template<class T> void read(int reg, T& buffer){
    buffer = *static_cast<T*>(csr[reg]);
  };

  // THIS NEEDS testing/debugging... doesn't really work as I originally planned
  // at moment.
  template<class T> void write(int reg, T * buffer){
    memcpy(csr[reg], buffer, csr_size[reg]);
  };

private:
  int _eeprom_counter, _reg_counter;


};


#endif
