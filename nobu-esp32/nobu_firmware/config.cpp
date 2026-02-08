#include "config.h"

const int DISPLAY_W = 122;
const int DISPLAY_H = 250;

const int EYE_Y = 70;
const int MOUTH_Y = 115;
const int LEFT_EYE_X = 40;
const int RIGHT_EYE_X = 82;
const int EYE_RADIUS = 10;
const int PUPIL_RADIUS = 4;

const char* EXPRESSIONS[] = {"happy", "sad", "thinking", "excited", "sleepy", "curious"};
const int NUM_EXPRESSIONS = 6;

const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

const char* WS_HOST = "192.168.1.100";
const uint16_t WS_PORT = 8765;
const char* WS_PATH = "/ws";

const char* DEVICE_ID = "nobu-kitchen";
const char* DEVICE_LOCATION = "kitchen";

const unsigned long WIFI_RECONNECT_MS = 5000;
const unsigned long WS_RECONNECT_MS = 3000;

const bool DEMO_MODE = true;
const unsigned long DEMO_CYCLE_MS = 6000;

