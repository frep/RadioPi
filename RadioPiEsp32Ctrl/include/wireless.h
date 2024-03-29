#ifndef WIRELESS_H
#define WIRELESS_H

#include <AsyncMqttClient.h>
#include <ESPAsyncWebServer.h>

void initWireless();
void connectToWifi();
void connectToMqtt();
void startWebserver();
void handleWebserver();
void WiFiEvent(WiFiEvent_t event);
void onMqttConnect(bool sessionPresent);
void onMqttDisconnect(AsyncMqttClientDisconnectReason reason);
void onMqttSubscribe(uint16_t packetId, uint8_t qos);
void onMqttUnsubscribe(uint16_t packetId);
void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total);
void onMqttPublish(uint16_t packetId);

#endif