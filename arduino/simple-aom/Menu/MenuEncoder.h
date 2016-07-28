

#ifndef MENUENCODER_H
#define MENUENCODER_H

#include "Arduino.h"
//#include "digitalWriteFast.h"

#define ENC_SW 21
#define ENC_A 22
#define ENC_B 23

#define HOLD_TIMEOUT 300
#define PUSH_TIMEOUT 50
#define CHEAT_CODE "PP-+-+PP"
#define CHEAT_LEN 8

struct Encoder {
  volatile int16_t pos = 0;
  volatile bool flag = 0;
  volatile unsigned long millis_down = 0;
  volatile unsigned long millis_up = 0;
} LCDENC;


void pollEncoder(){
  digitalReadFast(ENC_B) ? LCDENC.pos++ : LCDENC.pos--;
}

void pollEncoderSW(){
  if(digitalReadFast(ENC_SW)){
    LCDENC.millis_up = millis();
    LCDENC.flag = 1;
  } else {
    LCDENC.millis_down = millis();
    LCDENC.flag = 0;
  }

}

// void pollEncoderSW2(){
//   LCDENC.millis_up = millis();
//   LCDENC.flag = 1;
// }

class EncoderStream : public Stream {
public:

EncoderStream(Encoder & enc) : enc(enc){};

  Encoder & enc;
  int oldPos;

  void begin(){
    _cheat_idx = CHEAT_LEN;
    strcpy(_cheat_code, CHEAT_CODE);

    pinMode(ENC_A, INPUT_PULLUP);
    pinMode(ENC_B, INPUT_PULLUP);
    pinMode(ENC_SW, INPUT_PULLUP);

    attachInterrupt(ENC_A, pollEncoder, FALLING);
    attachInterrupt(ENC_SW, pollEncoderSW, CHANGE);
  //  attachInterrupt(ENC_SW, pollEncoderSW2, RISING);
    // // set up interrupts for both INT0, 1 only...
    // //EICRA = _BV(3);
    // EICRA = 0x07; // binary 0b0110; INT0 on falling edge, INT1 on change
    // EIMSK = 0x03;
  }

  int available() {
      return (enc.pos != oldPos || enc.flag);
  }

  int peek() {
    if (enc.pos < oldPos) return '-';
    if (enc.pos > oldPos) return '+';
    if (enc.flag) {
      if ((enc.millis_up - enc.millis_down) > HOLD_TIMEOUT) return 'H';
      if ((enc.millis_up - enc.millis_down) > PUSH_TIMEOUT) return 'P';
    }
     // what about hold?
    // need to fix this!! FIX
    //if (_push_action || (PIND & _BV(6))) return _push_action;
    return -1;
  }

  int read() {
    if (!strcmp(_cheat, _cheat_code)){
      Serial.println("cheat activated!!");
      Serial.println(_cheat);
      flush_buffer();
      return 'C';
    }
    if (enc.pos < oldPos) {
      //oldPos = _buff_en ? oldPos + 1 : enc.pos;
      oldPos = enc.pos;
      //_cheat[_cheat_idx++ % CHEAT_LEN] = '-';
      cheat_buffer('-');
      Serial.println(_cheat);
      return '-';
    }
    if (enc.pos > oldPos) {
      //oldPos = _buff_en ? oldPos + 1 : enc.pos;
      oldPos = enc.pos;
      //_cheat[_cheat_idx++ % CHEAT_LEN] = '+';
      cheat_buffer('+');
      Serial.println(_cheat);
      return '+';
    }

    if (enc.flag) {
      enc.flag = 0;
      if ((enc.millis_up - enc.millis_down) > HOLD_TIMEOUT) {
        flush_buffer();
        Serial.println(_cheat);
        return 'H';
      }
      if ((enc.millis_up - enc.millis_down) > PUSH_TIMEOUT){
        cheat_buffer('P');
        //_cheat[_cheat_idx++ % CHEAT_LEN] = 'P';
        Serial.println(_cheat);
        return 'P';
      }
    }

    // push button... if encoder not rotating
    //if (!digitalRead(ENC_SW) && ! _sw_pushed){
    // custom... switch on PD6

    // switch state: bit 1 = ~ switch; bit 0 = internal switch state tracker
    // note: because of pullup, switch -> 1 = off, switch -> 0 = on
    //  0 0 -> pushed, wasn't pushed last time = pushed state
    //  0 1 -> pushed, was pushed last time = hold vs push state timeout
    //  1 0 -> not pushed, wasn't pushed last time = nothing
    //  1 1 -> not pushed, was pushed last time = released state
    // bool switch_state = PIND & _BV(6);
    // int state = (switch_state << 1) | _sw_pushed;
    // int curr_millis;
    // Serial.print("State ");
    // Serial.println(state);
    //
    // switch (state){
    //   case 0:
    //     // pushed, wasn't pushed last time; change to pushed state
    //     _sw_pushed = true;
    //     _push_millis = millis();
    //     break;
    //   case 1:
    //     // pushed, was pushed last time; check if holding or pushing
    //     curr_millis = millis();
    //     if (curr_millis - _push_millis > HOLD_TIMEOUT){
    //       _push_action = 'H';
    //     } else if (curr_millis - _push_millis > PUSH_TIMEOUT){
    //       _push_action = 'P';
    //     } else {
    //       _push_action = 0;
    //     }
    //     break;
    //   //case 2:
    //     // not pushed, wasn't pushed last time so do nothing
    //     //break;
    //   case 3:
    //     // not pushed, was pushed last time so release state
    //     int action = _push_action;
    //     _push_action = 0;
    //
    //     return action;
    //     //break;
    // }

    return -1;
  }


  void flush() {
    oldPos = enc.pos;
    enc.flag = 0;
  }

  size_t write(uint8_t val) {
    oldPos = val;
    enc.pos = val;  // update encoder to this position as well...
    return 1;
  }

  void cheat_buffer(char c){
    if (_cheat_idx < CHEAT_LEN){
      _cheat[_cheat_idx++] = c;
      _cheat[_cheat_idx] = '\0';
    }
    Serial.print("index: ");
    Serial.println(_cheat_idx);
    Serial.println(_cheat);
  }

  void flush_buffer(){
    _cheat_idx = 0;
    _cheat[_cheat_idx] = '\0';
  }

private:
  int _a, _b, _sw;
  char _cheat[CHEAT_LEN + 1];
  char _cheat_code[CHEAT_LEN];
  int _cheat_idx;

};


#endif
