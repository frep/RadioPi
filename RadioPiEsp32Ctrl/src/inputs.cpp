#include <inputs.h>
#include <Arduino.h>
#include <SimpleRotary.h>
#include <AsyncMqttClient.h>
#include <Debug.h>

extern SimpleRotary rotary;
extern AsyncMqttClient mqttClient;

void checkRotaryEncoder()
{
  // check for rotation
  switch(rotary.rotate())
  {
    case 1:
    // clockwise rotation detected
    DEBUG_P("CW");
    mqttClient.publish("rotaryEncoder", 0, true, "CW");
    break;
    case 2:
    // counter-clockwise rotation detected
    DEBUG_P("CCW");
    mqttClient.publish("rotaryEncoder", 0, true, "CCW");
    break;
    default:
    // no rotation detected
    break;
  }
  // check for button presses
  switch(rotary.pushType(1000)) // number of milliseconds button has to be pushed for it to be considered a long push.
  {
    case 1:
    // normal button press detected
    DEBUG_P("Pushed");
    mqttClient.publish("rotaryEncoder", 0, true, "Pushed");
    break;
    case 2:
    // long button press detected
    DEBUG_P("Long Pushed");
    mqttClient.publish("rotaryEncoder", 0, true, "LongPushed");
    break;
    default:
    // no button press detected
    break;
  }
}