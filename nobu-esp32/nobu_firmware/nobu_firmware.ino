#include "config.h"
#include "display_faces.h"
#include "ws_client.h"

#include <SD.h>
#include <SPI.h>

// SD card SPI instance
SPIClass sdSPI(HSPI);

// Button debounce
static unsigned long lastButtonPress = 0;
const int DEBOUNCE_MS = 300;

static int currentExpression = 0;
static unsigned long lastDemoCycleMs = 0;

static void handleMessage(const char* mood, const char* text) {
  if (mood && strlen(mood) > 0) {
    displayShowExpression(mood);
  }

  if (text && strlen(text) > 0) {
    Serial.print("WS text: ");
    Serial.println(text);
  }
}

static void initSdCard() {
  Serial.println("Initializing SD card...");
  sdSPI.begin(SD_SCLK, SD_MISO, SD_MOSI, SD_CS);
  if (SD.begin(SD_CS, sdSPI, 27000000)) {
    displaySetSdAvailable(true);
    Serial.println("SD card initialized!");
    if (SD.exists("/faces")) {
      Serial.println("Found /faces folder - will use bitmap faces");
    } else {
      Serial.println("No /faces folder - will use programmatic drawing");
    }
  } else {
    displaySetSdAvailable(false);
    Serial.println("SD card initialization failed - using programmatic drawing");
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("\n=== Nobu Firmware ===");

  pinMode(BUTTON_PIN, INPUT);

  initSdCard();
  displayInit();
  displayShowExpression(EXPRESSIONS[currentExpression]);

  wsInit(handleMessage);
}

void loop() {
  wsPoll();

  if (DEMO_MODE && !wsIsConnected()) {
    unsigned long now = millis();
    if (now - lastDemoCycleMs > DEMO_CYCLE_MS) {
      lastDemoCycleMs = now;
      currentExpression = (currentExpression + 1) % NUM_EXPRESSIONS;
      displayShowExpression(EXPRESSIONS[currentExpression]);
    }
  }

  if (digitalRead(BUTTON_PIN) == LOW) {
    if (millis() - lastButtonPress > DEBOUNCE_MS) {
      lastButtonPress = millis();
      currentExpression = (currentExpression + 1) % NUM_EXPRESSIONS;
      displayShowExpression(EXPRESSIONS[currentExpression]);
    }
  }

  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    input.toLowerCase();
    if (input.length() > 0) {
      displayShowExpression(input.c_str());
    }
  }

  delay(50);
}
