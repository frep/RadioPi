#include <Arduino.h>
#include <AsyncMqttClient.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h> 
#include <wireless.h>
#include <Debug.h>
#include <MqttCredentials.h>
#include <peripherals.h>

extern AsyncWiFiManager wifiManager;
extern AsyncMqttClient mqttClient;
extern TimerHandle_t mqttReconnectTimer;
extern TimerHandle_t wifiReconnectTimer;

extern bool bPendingAliveRequest;
extern uint nUnansweredAliveRequests;
extern RpiState state;

void connectToWifi() 
{
  DEBUG_P("Connecting to Wi-Fi...");
  wifiManager.autoConnect("AutoConnectAP");
}

void connectToMqtt() 
{
  DEBUG_P("Connecting to MQTT...");
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
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

  // Meldungen fuer den Esp32 abonnieren
  mqttClient.subscribe("radioPiEsp32", 0);
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

  if(!strcmp(topic, "radioPiEsp32"))
  {
      if(!strcmp(payload, "alive"))
      {
        DEBUG_P("radioPiEsp32: alive");
        if(state == rpiStartup)
        {
          // startup has finished
          state = rpiUp;
        }
        // a pending alive request was answered
        bPendingAliveRequest = false;
        nUnansweredAliveRequests = 0;
      }
  }

}

void onMqttPublish(uint16_t packetId) 
{
  DEBUG_P("Publish acknowledged.");
  DEBUG_T("  packetId: ");
  DEBUG_P(packetId);
}