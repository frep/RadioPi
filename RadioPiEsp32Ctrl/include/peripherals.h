#ifndef INPUTS_H
#define INPUTS_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

// Raspberry Pi states
enum RpiState : uint8_t 
{
  rpiDown,
  rpiStartup,
  rpiUp,
  rpiShutdown,   
};

/////////////////////////////////////////////////////////////////////////////////////
// times in milliseconds
/////////////////////////////////////////////////////////////////////////////////////

#define longpress        1000      
#define isAliveInterval  5000
#define shutdownTime    30000

/////////////////////////////////////////////////////////////////////////////////////
// Hardware: feather pins
/////////////////////////////////////////////////////////////////////////////////////

// SwitchEncoder
#define Channel_A 17
#define Channel_B 21
#define Switch    16

// Neopixel
#define PIN_NEO   14
#define NUMPIXELS  8

// startpin (to switch the relay)
#define PIN_POWER 32
#define PIN_LED   13  // the onboard-LED indicates, if relay is ON

/////////////////////////////////////////////////////////////////////////////////////
// Raspberry Pi control functions
/////////////////////////////////////////////////////////////////////////////////////
void startup();
void shutdown();
void isRpiAlive();
void isRpiDown();
void handleState(RpiState actualState);

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
  Neopixelstick(uint16_t anzahlPixel, uint16_t pinNummer);
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