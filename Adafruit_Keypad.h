#ifndef _ADAFRUIT_KEYPAD_H_
#define _ADAFRUIT_KEYPAD_H_

#include "Arduino.h"

#define makeKeymap(x) ((byte*)x)

class Adafruit_Keypad {
public:
    Adafruit_Keypad(byte *userKeymap, byte *row, byte *col, int numRows, int numCols);
    ~Adafruit_Keypad();
    void begin();

    void read();

    bool justPressed(byte key);
    bool justReleased(byte key);
    bool isPressed(byte key);
    bool isReleased(byte key);

private:
    byte *_userKeymap;
    byte *_row;
    byte *_col;
    volatile byte *_keystates;

    int _numRows;
    int _numCols;

    volatile byte *getKeyState(byte key);
};

#endif