#include <Arduino.h>

#include <AsyncMqttClient.h>
#include <MqttCredentials.h>

#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>         //https://github.com/tzapu/WiFiManager

#include <Debug.h>

// Async MQTT
AsyncMqttClient mqttClient;
TimerHandle_t mqttReconnectTimer;
TimerHandle_t wifiReconnectTimer;
// WifiManager
AsyncWebServer server(80);
DNSServer dns;

void connectToWifi() 
{
  DEBUG_P("Connecting to Wi-Fi...");
  //WiFiManager
  AsyncWiFiManager wifiManager(&server,&dns);
  wifiManager.autoConnect("AutoConnectAP");
}

void connectToMqtt() 
{
  DEBUG_P("Connecting to MQTT...");
  mqttClient.connect();
}

void WiFiEvent(WiFiEvent_t event) 
{
    DEBUG_F("[WiFi-event] event: %d\n", event);
    switch(event) 
    {
      case SYSTEM_EVENT_STA_GOT_IP:
        DEBUG_P("WiFi connected");
        DEBUG_P("IP address: ");
        DEBUG_P(WiFi.localIP());
        connectToMqtt();
        break;
      case SYSTEM_EVENT_STA_DISCONNECTED:
        DEBUG_P("WiFi lost connection");
        xTimerStop(mqttReconnectTimer, 0); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
		    xTimerStart(wifiReconnectTimer, 0);
        break;
      default:
        // nothing to do yet...
        break;
    }
}

void onMqttConnect(bool sessionPresent) 
{
  DEBUG_P("Connected to MQTT.");
  DEBUG_T("Session present: ");
  DEBUG_P(sessionPresent);
  uint16_t packetIdSub = mqttClient.subscribe("test/lol", 2);
  DEBUG_T("Subscribing at QoS 2, packetId: ");
  DEBUG_P(packetIdSub);
  mqttClient.publish("test/lol", 0, true, "test 1");
  DEBUG_P("Publishing at QoS 0");
  uint16_t packetIdPub1 = mqttClient.publish("test/lol", 1, true, "test 2");
  DEBUG_T("Publishing at QoS 1, packetId: ");
  DEBUG_P(packetIdPub1);
  uint16_t packetIdPub2 = mqttClient.publish("test/lol", 2, true, "test 3");
  DEBUG_T("Publishing at QoS 2, packetId: ");
  DEBUG_P(packetIdPub2);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) 
{
  DEBUG_P("Disconnected from MQTT.");

  if (WiFi.isConnected()) 
  {
    xTimerStart(mqttReconnectTimer, 0);
  }
}

void onMqttSubscribe(uint16_t packetId, uint8_t qos) 
{
  DEBUG_P("Subscribe acknowledged.");
  DEBUG_T("  packetId: ");
  DEBUG_P(packetId);
  DEBUG_T("  qos: ");
  DEBUG_P(qos);
}

void onMqttUnsubscribe(uint16_t packetId) 
{
  DEBUG_P("Unsubscribe acknowledged.");
  DEBUG_T("  packetId: ");
  DEBUG_P(packetId);
}

void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) 
{
  DEBUG_P("Publish received.");
  DEBUG_T("  topic: ");
  DEBUG_P(topic);
  DEBUG_T("  qos: ");
  DEBUG_P(properties.qos);
  DEBUG_T("  dup: ");
  DEBUG_P(properties.dup);
  DEBUG_T("  retain: ");
  DEBUG_P(properties.retain);
  DEBUG_T("  len: ");
  DEBUG_P(len);
  DEBUG_T("  index: ");
  DEBUG_P(index);
  DEBUG_T("  total: ");
  DEBUG_P(total);
}

void onMqttPublish(uint16_t packetId) 
{
  DEBUG_P("Publish acknowledged.");
  DEBUG_T("  packetId: ");
  DEBUG_P(packetId);
}

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

  connectToWifi();
}

void loop() 
{
}