#include "ws_client.h"
#include "config.h"

#include <WiFi.h>
#include <ArduinoWebsockets.h>
#include <ArduinoJson.h>

using namespace websockets;

static WebsocketsClient wsClient;
static bool wsConnected = false;
static unsigned long lastWifiAttemptMs = 0;
static unsigned long lastWsAttemptMs = 0;
static WsMessageHandler messageHandler = nullptr;

static void sendHello() {
  StaticJsonDocument<256> doc;
  doc["type"] = "hello";
  doc["device_id"] = DEVICE_ID;
  doc["location"] = DEVICE_LOCATION;

  String payload;
  serializeJson(doc, payload);
  wsClient.send(payload);
}

static void handleWsMessage(const String& message) {
  StaticJsonDocument<512> doc;
  DeserializationError err = deserializeJson(doc, message);
  if (err) {
    Serial.print("WS JSON parse error: ");
    Serial.println(err.c_str());
    return;
  }

  const char* mood = doc["mood"];
  const char* text = doc["text"];

  if (messageHandler) {
    messageHandler(mood, text);
  }
}

static void connectWifiIfNeeded() {
  if (WiFi.status() == WL_CONNECTED) {
    return;
  }

  unsigned long now = millis();
  if (now - lastWifiAttemptMs < WIFI_RECONNECT_MS) {
    return;
  }

  lastWifiAttemptMs = now;
  Serial.println("Connecting to WiFi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

static void connectWsIfNeeded() {
  if (WiFi.status() != WL_CONNECTED) {
    return;
  }

  if (wsConnected) {
    return;
  }

  unsigned long now = millis();
  if (now - lastWsAttemptMs < WS_RECONNECT_MS) {
    return;
  }

  lastWsAttemptMs = now;
  String url = String("ws://") + WS_HOST + ":" + WS_PORT + WS_PATH;
  Serial.print("Connecting WS: ");
  Serial.println(url);
  wsClient.connect(url);
}

void wsInit(WsMessageHandler handler) {
  messageHandler = handler;

  wsClient.onMessage([&](WebsocketsMessage msg) {
    handleWsMessage(msg.data());
  });

  wsClient.onEvent([&](WebsocketsEvent event, String data) {
    if (event == WebsocketsEvent::ConnectionOpened) {
      wsConnected = true;
      Serial.println("WebSocket connected");
      sendHello();
    } else if (event == WebsocketsEvent::ConnectionClosed) {
      wsConnected = false;
      Serial.println("WebSocket disconnected");
    } else if (event == WebsocketsEvent::GotPing) {
      Serial.println("WebSocket ping");
    } else if (event == WebsocketsEvent::GotPong) {
      Serial.println("WebSocket pong");
    }
  });
}

void wsPoll() {
  connectWifiIfNeeded();
  connectWsIfNeeded();

  if (WiFi.status() == WL_CONNECTED) {
    wsClient.poll();
  }
}

bool wsIsConnected() {
  return wsConnected;
}

bool wifiIsConnected() {
  return WiFi.status() == WL_CONNECTED;
}

