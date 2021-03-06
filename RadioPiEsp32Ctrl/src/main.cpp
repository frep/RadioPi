#include <Arduino.h>
#include <AsyncMqttClient.h>
#include <wireless.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>      // https://github.com/tzapu/WiFiManager
#include <SwitchEncoder.h>
#include <peripherals.h>
#include <Debug.h>

// Async MQTT
AsyncMqttClient mqttClient;
TimerHandle_t mqttReconnectTimer;
TimerHandle_t wifiReconnectTimer;
// WifiManager
AsyncWebServer server(80);
DNSServer dns;
AsyncWiFiManager wifiManager(&server,&dns);
// Rotary Encoder
SwitchEncoder encoder = SwitchEncoder(Channel_A, Channel_B, Switch);
// Neopixel
Neopixelstick pixels = Neopixelstick(NUMPIXELS, PIN_NEO);
// RaspberryPi state
RpiState state;

bool bPendingAliveRequest;
uint nUnansweredAliveRequests;
bool bWebserverStarted;

void setup() 
{
  DEBUG_B(115200);
  DEBUG_P();
  DEBUG_P();

  state = rpiDown;
  bPendingAliveRequest = false;
  nUnansweredAliveRequests = 0;
  bWebserverStarted = false;
  pinMode(PIN_POWER, OUTPUT);
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_POWER, LOW);
  digitalWrite(PIN_LED, LOW);

  mqttReconnectTimer = xTimerCreate("mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToMqtt));
  wifiReconnectTimer = xTimerCreate("wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToWifi));

  WiFi.onEvent(WiFiEvent);

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onSubscribe(onMqttSubscribe);
  mqttClient.onUnsubscribe(onMqttUnsubscribe);
  mqttClient.onMessage(onMqttMessage);
  mqttClient.onPublish(onMqttPublish);

  pixels.setup();

  encoder.init();
  encoder.attachFunctionOnButtonPressedEdge(buttonPressed);
  encoder.attachFunctionOnButtonReleasedEdge(buttonReleased);
  encoder.attachFunctionOnClockwiseTurn(turnClockwise);
  encoder.attachFunctionOnCounterClockwiseTurn(turnCounterclockwise);

  connectToWifi();
}

void loop() 
{
  encoder.check();
  pixels.check(millis());
  handleState(state);
  handleWebserver();
}


  

