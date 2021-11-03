#include <Arduino.h>
#include <wireless.h>
#include <peripherals.h>
#include <Debug.h>

void setup() 
{
  DEBUG_B(115200);
  DEBUG_P();
  DEBUG_P();

  initWireless();

  initPeripherals();

}

void loop() 
{
  handleWebserver();
  loopPeripherals();
}


  

