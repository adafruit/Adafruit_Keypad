#include "Adafruit_Keypad.h"

#define _KEY_PRESSED_POS (1)
#define _KEY_PRESSED     (1UL << _KEY_PRESSED_POS)

#define _JUST_PRESSED_POS (2)
#define _JUST_PRESSED     (1UL << _JUST_PRESSED_POS)

#define _JUST_RELEASED_POS (3)
#define _JUST_RELEASED     (1UL << _JUST_RELEASED_POS)

#define _KEYPAD_SETTLING_DELAY 20

Adafruit_Keypad::Adafruit_Keypad(byte *userKeymap, byte *row, byte *col, int numRows, int numCols)
{
    _userKeymap = userKeymap;
    _row = row;
    _col = col;
    _numRows = numRows;
    _numCols = numCols;

    _keystates = NULL;
}

Adafruit_Keypad::~Adafruit_Keypad(){
    if(_keystates != NULL){
        free((void *)_keystates);
    }
}

volatile byte *Adafruit_Keypad::getKeyState(byte key)
{
    for(int i=0; i<_numRows*_numCols; i++){
        if(_userKeymap[i] == key){
            return _keystates + i;
        }
    }
    return NULL;
}

void Adafruit_Keypad::tick() {
    uint8_t evt;
    for(int i=0; i<_numRows; i++){
        digitalWrite(_row[i], LOW);
    }

    int i = 0;
    for(int r=0; r<_numRows; r++){
        digitalWrite(_row[r], HIGH);
        delayMicroseconds(_KEYPAD_SETTLING_DELAY);
        for(int c=0; c<_numCols; c++){
            bool pressed = digitalRead(_col[c]);
            //Serial.print((int)pressed);
            volatile byte *state = _keystates + i;
            byte currentState = *state;
            if(pressed && !(currentState & _KEY_PRESSED)){
                currentState |= (_JUST_PRESSED | _KEY_PRESSED);
                evt = KEY_JUST_PRESSED;
                _eventbuf.store_char(evt);
                _eventbuf.store_char(*(_userKeymap + i));
            }
            else if(!pressed && (currentState & _KEY_PRESSED)){
                currentState |= _JUST_RELEASED;
                currentState &= ~(_KEY_PRESSED);
                evt = KEY_JUST_RELEASED;
                _eventbuf.store_char(evt);
                _eventbuf.store_char(*(_userKeymap + i));
            }
            *state = currentState;
            i++;
        }
        //Serial.println("");
        digitalWrite(_row[r], LOW);
    }
}

void Adafruit_Keypad::begin()
{
    _keystates = (volatile byte *)malloc(_numRows * _numCols);
    memset((void *)_keystates, 0, _numRows * _numCols);

    for(int i=0; i<_numRows; i++){
        pinMode(_row[i], OUTPUT);
        digitalWrite(_row[i], LOW);
    }

    for(int i=0; i<_numCols; i++){
        pinMode(_col[i], INPUT_PULLDOWN);
    }
}

bool Adafruit_Keypad::justPressed(byte key)
{
    volatile byte *state = getKeyState(key);
    bool val = (*state & _JUST_PRESSED) != 0;

    *state &= ~(_JUST_PRESSED);

    return val;
}

bool Adafruit_Keypad::justReleased(byte key)
{
    volatile byte *state = getKeyState(key);
    bool val = (*state & _JUST_RELEASED) != 0;

    *state &= ~(_JUST_RELEASED);

    return val;
}

bool Adafruit_Keypad::isPressed(byte key)
{
    return (*getKeyState(key) & _KEY_PRESSED) != 0;
}

bool Adafruit_Keypad::isReleased(byte key)
{
    return (*getKeyState(key) & _KEY_PRESSED) == 0;
}

int Adafruit_Keypad::available()
{
    return (_eventbuf.available() >> 1);
}

keypadEvent Adafruit_Keypad::read()
{
    keypadEvent k;
    k.bit.EVENT = _eventbuf.read_char();
    k.bit.KEY = _eventbuf.read_char();

    return k;
}