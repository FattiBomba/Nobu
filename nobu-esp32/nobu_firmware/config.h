#pragma once
#include <cstdint>

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
extern const int DISPLAY_W;
extern const int DISPLAY_H;

// Face positioning (smaller face, mouth closer to eyes)
extern const int EYE_Y;
extern const int MOUTH_Y;
extern const int LEFT_EYE_X;
extern const int RIGHT_EYE_X;
extern const int EYE_RADIUS;
extern const int PUPIL_RADIUS;

// Expressions
extern const char* EXPRESSIONS[];
extern const int NUM_EXPRESSIONS;

// WiFi credentials
extern const char* WIFI_SSID;
extern const char* WIFI_PASSWORD;

// WebSocket config
extern const char* WS_HOST;
extern const uint16_t WS_PORT;
extern const char* WS_PATH;

// Device identity
extern const char* DEVICE_ID;
extern const char* DEVICE_LOCATION;

// Reconnect intervals
extern const unsigned long WIFI_RECONNECT_MS;
extern const unsigned long WS_RECONNECT_MS;

// Demo mode
extern const bool DEMO_MODE;
extern const unsigned long DEMO_CYCLE_MS;
