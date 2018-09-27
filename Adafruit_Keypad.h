#ifndef _ADAFRUIT_KEYPAD_H_
#define _ADAFRUIT_KEYPAD_H_

#include "Arduino.h"
#include "RingBuffer.h"

#define makeKeymap(x) ((byte*)x)

#define KEY_JUST_RELEASED (0)
#define KEY_JUST_PRESSED (1)

union keypadEvent {
    struct {
        uint8_t KEY: 8; ///< the keycode
        uint8_t EVENT: 8; ///< the edge
    } bit; ///< bitfield format
    uint16_t reg; ///< register format
};

class Adafruit_Keypad {
public:
    Adafruit_Keypad(byte *userKeymap, byte *row, byte *col, int numRows, int numCols);
    ~Adafruit_Keypad();
    void begin();

    void tick();

    bool justPressed(byte key);
    bool justReleased(byte key);
    bool isPressed(byte key);
    bool isReleased(byte key);
    int available();
    keypadEvent read();

private:
    byte *_userKeymap;
    byte *_row;
    byte *_col;
    volatile byte *_keystates;
    RingBuffer _eventbuf;

    int _numRows;
    int _numCols;

    volatile byte *getKeyState(byte key);
};

#endif