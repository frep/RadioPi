#ifndef INPUTS_H
#define INPUTS_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

/////////////////////////////////////////////////////////////////////////////////////
// Hardware: feather pins
/////////////////////////////////////////////////////////////////////////////////////

// SwitchEncoder
#define Channel_A 17
#define Channel_B 21
#define Switch    16

// Neopixel
#define PIN_NEO   14
#define NUMPIXELS  1    // later: 8

// startpin (to switch the relay)
#define PIN_POWER 32

/////////////////////////////////////////////////////////////////////////////////////
// Switch Encoder functions
/////////////////////////////////////////////////////////////////////////////////////

void buttonPressed();
void buttonReleased();
void turnClockwise();
void turnCounterclockwise();

/////////////////////////////////////////////////////////////////////////////////////
// class Neopixelstick
/////////////////////////////////////////////////////////////////////////////////////

class Neopixelstick
{
public:
	Neopixelstick();
    ~Neopixelstick();

    void setup();
    void startFadeIn();
    void check(unsigned long millis);
		
private:
    unsigned long m_previousMillis;
    unsigned long m_startLightOn;
    unsigned long m_lightOnTime;
    uint8_t m_brightness;
    bool m_bfadeIn;
    bool m_bfadeOut;
    uint8_t m_numPixels;
    Adafruit_NeoPixel* m_pPixels;

    void setBrightness(uint8_t brightness);
};

#endif