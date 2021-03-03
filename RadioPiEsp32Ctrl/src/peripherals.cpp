#include <peripherals.h>
#include <Arduino.h>
#include <SimpleRotary.h>
#include <AsyncMqttClient.h>
#include <Adafruit_NeoPixel.h>
#include <Debug.h>

extern AsyncMqttClient mqttClient;
extern Neopixelstick pixels;

/////////////////////////////////////////////////////////////////////////////////////
// Switch Encoder functions
/////////////////////////////////////////////////////////////////////////////////////

void buttonPressed()
{
    DEBUG_P("Pressed");
    mqttClient.publish("volumio", 0, true, "buttonPressed");
    pixels.startFadeIn();
}

void buttonReleased()
{
    DEBUG_P("Released");
    //mqttClient.publish("volumio", 0, true, "buttonReleased");
}

void turnClockwise()
{
    DEBUG_P("CW");
    mqttClient.publish("volumio", 0, true, "turnCW");
    pixels.startFadeIn();
}

void turnCounterclockwise()
{
    DEBUG_P("CCW");
    mqttClient.publish("volumio", 0, true, "turnCCW");
    pixels.startFadeIn();
}


/////////////////////////////////////////////////////////////////////////////////////
// class Neopixelstick
/////////////////////////////////////////////////////////////////////////////////////

Neopixelstick::Neopixelstick()
{
    m_previousMillis = 0;
    m_startLightOn = 0;
    m_lightOnTime = 5000;
    m_brightness = 0;
    m_bfadeIn = false;
    m_bfadeOut = false;
    m_numPixels = NUMPIXELS;
    m_pPixels = new Adafruit_NeoPixel(NUMPIXELS, PIN_NEO, NEO_GRB + NEO_KHZ800);
}

Neopixelstick::~Neopixelstick()
{
    delete m_pPixels;
}

void Neopixelstick::setup()
{
    m_pPixels->begin();
}

void Neopixelstick::startFadeIn()
{
    m_bfadeIn = true;
}

void Neopixelstick::check(unsigned long millis)
{
    if(millis - m_previousMillis >= 10)
    {
        m_previousMillis = millis;
        // light is fading in
        if(m_bfadeIn)
        {
            m_bfadeOut = false;
            if(m_brightness < 255)
            {
                m_brightness++;
                setBrightness(m_brightness);
            }
            else
            {
                m_bfadeIn = false;
                m_startLightOn = millis;
            }
        }
        // light on
        if((m_startLightOn > 0) && (millis - m_startLightOn > m_lightOnTime))
        {
            m_bfadeOut = true;  
        }
        // light is fading out
        if(m_bfadeOut)
        {
            if(m_brightness > 0)
            {
                m_brightness--;
                setBrightness(m_brightness);
            }
            else
            {
                m_bfadeOut = false;
                m_startLightOn = 0;
            }
        }
    }
}

void Neopixelstick::setBrightness(uint8_t brightness)
{
    for(int i=0; i<m_numPixels; i++)
    {
      // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
      m_pPixels->setPixelColor(i, m_pPixels->Color(brightness,brightness,brightness));
      m_pPixels->show(); // This sends the updated pixel color to the hardware.
    } 
}