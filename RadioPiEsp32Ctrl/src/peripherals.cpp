#include <peripherals.h>
#include <Arduino.h>
#include <SimpleRotary.h>
#include <AsyncMqttClient.h>
#include <Adafruit_NeoPixel.h>
#include <Debug.h>

extern AsyncMqttClient mqttClient;
extern Adafruit_NeoPixel pixels;

/////////////////////////////////////////////////////////////////////////////////////
// Switch Encoder functions
/////////////////////////////////////////////////////////////////////////////////////

void buttonPressed()
{
    DEBUG_P("Pressed");
    mqttClient.publish("rotaryEncoder", 0, true, "Pressed");
}

void buttonReleased()
{
    DEBUG_P("Released");
    mqttClient.publish("rotaryEncoder", 0, true, "Released");
}

void turnClockwise()
{
    DEBUG_P("CW");
    mqttClient.publish("rotaryEncoder", 0, true, "CW");
}

void turnCounterclockwise()
{
    DEBUG_P("CCW");
    mqttClient.publish("rotaryEncoder", 0, true, "CCW");
}


/////////////////////////////////////////////////////////////////////////////////////
// Neopixel functions
/////////////////////////////////////////////////////////////////////////////////////

void setBrightness(uint8_t brightness)
{
    for(int i=0;i<NUMPIXELS;i++)
    {
      // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
      pixels.setPixelColor(i, pixels.Color(brightness,brightness,brightness));
      pixels.show(); // This sends the updated pixel color to the hardware.
    } 
}