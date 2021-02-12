#include <Arduino.h>
#include <AsyncMqttClient.h>
#include <MqttCredentials.h>
#include <wireless.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>      // https://github.com/tzapu/WiFiManager
#include <SimpleRotary.h>
#include <inputs.h>
#include <Adafruit_NeoPixel.h>
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
SimpleRotary rotary(Channel_A, Channel_B, Switch);
Adafruit_NeoPixel pixels(1, 19, NEO_GRB + NEO_KHZ800);

void setup() 
{
  DEBUG_B(115200);
  DEBUG_P();
  DEBUG_P();

  mqttReconnectTimer = xTimerCreate("mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToMqtt));
  wifiReconnectTimer = xTimerCreate("wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToWifi));

  WiFi.onEvent(WiFiEvent);

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onSubscribe(onMqttSubscribe);
  mqttClient.onUnsubscribe(onMqttUnsubscribe);
  mqttClient.onMessage(onMqttMessage);
  mqttClient.onPublish(onMqttPublish);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);

  pixels.begin();

  connectToWifi();
}

void loop() 
{
  checkRotaryEncoder();
  pixels.clear();
  pixels.setPixelColor(0, pixels.Color(0, 150, 0));
  pixels.show(); 
}
