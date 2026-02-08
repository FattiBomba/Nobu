#pragma once

// TTGO T5 V2.3 2.13" display pinout
#define EPD_CS    5
#define EPD_DC    17
#define EPD_RST   16
#define EPD_BUSY  4

// SD Card pins for TTGO T5 V2.3 (HSPI)
#define SD_CS     2
#define SD_MOSI   15
#define SD_MISO   26
#define SD_SCLK   13

// Built-in button on TTGO T5
#define BUTTON_PIN 39

// Display dimensions (portrait mode - USB at bottom)
const int DISPLAY_W = 122;
const int DISPLAY_H = 250;

// Face positioning (smaller face, mouth closer to eyes)
const int EYE_Y = 70;
const int MOUTH_Y = 115;
const int LEFT_EYE_X = 40;
const int RIGHT_EYE_X = 82;
const int EYE_RADIUS = 10;
const int PUPIL_RADIUS = 4;

// Expressions
const char* EXPRESSIONS[] = {"happy", "sad", "thinking", "excited", "sleepy", "curious"};
const int NUM_EXPRESSIONS = 6;

// WiFi credentials
const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

// WebSocket config
const char* WS_HOST = "192.168.1.100";
const uint16_t WS_PORT = 8765;
const char* WS_PATH = "/ws";

// Device identity
const char* DEVICE_ID = "nobu-kitchen";
const char* DEVICE_LOCATION = "kitchen";

// Reconnect intervals
const unsigned long WIFI_RECONNECT_MS = 5000;
const unsigned long WS_RECONNECT_MS = 3000;

