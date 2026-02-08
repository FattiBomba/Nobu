#include "display_faces.h"
#include "config.h"

#include <GxEPD2_BW.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <SD.h>
#include <SPI.h>

// 2.13" e-paper display (250x122)
static GxEPD2_BW<GxEPD2_213_BN, GxEPD2_213_BN::HEIGHT> display(
  GxEPD2_213_BN(EPD_CS, EPD_DC, EPD_RST, EPD_BUSY)
);

static bool sdCardAvailable = false;

// Read a 16-bit little-endian value from file
static uint16_t read16(File &f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read();
  ((uint8_t *)&result)[1] = f.read();
  return result;
}

// Read a 32-bit little-endian value from file
static uint32_t read32(File &f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read();
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read();
  return result;
}

// Draw a monochrome BMP from SD card
static bool drawBitmapFromSD(const char* filename, int16_t x, int16_t y) {
  File bmpFile;
  int bmpWidth, bmpHeight;
  uint8_t bmpDepth;
  uint32_t bmpImageoffset;
  uint32_t rowSize;
  bool flip = true;
  int w, h, row, col;
  uint8_t b;
  uint32_t pos = 0;

  if ((bmpFile = SD.open(filename)) == false) {
    Serial.print("File not found: ");
    Serial.println(filename);
    return false;
  }

  if (read16(bmpFile) != 0x4D42) {
    Serial.println("Invalid BMP signature");
    bmpFile.close();
    return false;
  }

  read32(bmpFile);
  read32(bmpFile);
  bmpImageoffset = read32(bmpFile);

  read32(bmpFile);
  bmpWidth = read32(bmpFile);
  bmpHeight = read32(bmpFile);

  if (read16(bmpFile) != 1) {
    bmpFile.close();
    return false;
  }

  bmpDepth = read16(bmpFile);
  if (read32(bmpFile) != 0) {
    Serial.println("BMP must be uncompressed");
    bmpFile.close();
    return false;
  }

  if (bmpHeight < 0) {
    bmpHeight = -bmpHeight;
    flip = false;
  }

  rowSize = (bmpWidth * bmpDepth / 8 + 3) & ~3;

  w = bmpWidth;
  h = bmpHeight;

  for (row = 0; row < h; row++) {
    if (flip) {
      pos = bmpImageoffset + (h - 1 - row) * rowSize;
    } else {
      pos = bmpImageoffset + row * rowSize;
    }
    bmpFile.seek(pos);

    for (col = 0; col < w; col++) {
      if (bmpDepth == 1) {
        if (col % 8 == 0) {
          b = bmpFile.read();
        }
        if (b & (0x80 >> (col % 8))) {
          display.drawPixel(x + col, y + row, GxEPD_WHITE);
        } else {
          display.drawPixel(x + col, y + row, GxEPD_BLACK);
        }
      } else if (bmpDepth == 24) {
        uint8_t blu = bmpFile.read();
        uint8_t grn = bmpFile.read();
        uint8_t red = bmpFile.read();
        if ((red + grn + blu) / 3 > 128) {
          display.drawPixel(x + col, y + row, GxEPD_WHITE);
        } else {
          display.drawPixel(x + col, y + row, GxEPD_BLACK);
        }
      }
    }
  }

  bmpFile.close();
  return true;
}

static bool showExpressionFromSD(const char* mood) {
  char filepath[32];
  snprintf(filepath, sizeof(filepath), "/faces/%s.bmp", mood);

  if (!SD.exists(filepath)) {
    return false;
  }

  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);
    drawBitmapFromSD(filepath, 0, 0);

    display.setFont(&FreeMonoBold9pt7b);
    int16_t tbx, tby; uint16_t tbw, tbh;
    display.getTextBounds(mood, 0, 0, &tbx, &tby, &tbw, &tbh);
    display.setCursor((DISPLAY_W - tbw) / 2, 230);
    display.print(mood);
  } while (display.nextPage());

  return true;
}

static void drawEyes(int leftPupilOffsetX, int leftPupilOffsetY,
                     int rightPupilOffsetX, int rightPupilOffsetY,
                     int eyeRadiusLeft = EYE_RADIUS, int eyeRadiusRight = EYE_RADIUS) {
  display.drawCircle(LEFT_EYE_X, EYE_Y, eyeRadiusLeft, GxEPD_BLACK);
  display.drawCircle(LEFT_EYE_X, EYE_Y, eyeRadiusLeft - 1, GxEPD_BLACK);
  display.fillCircle(LEFT_EYE_X + leftPupilOffsetX, EYE_Y + leftPupilOffsetY, PUPIL_RADIUS, GxEPD_BLACK);

  display.drawCircle(RIGHT_EYE_X, EYE_Y, eyeRadiusRight, GxEPD_BLACK);
  display.drawCircle(RIGHT_EYE_X, EYE_Y, eyeRadiusRight - 1, GxEPD_BLACK);
  display.fillCircle(RIGHT_EYE_X + rightPupilOffsetX, EYE_Y + rightPupilOffsetY, PUPIL_RADIUS, GxEPD_BLACK);
}

static void drawSmile(int width, int depth, int thickness = 2) {
  for (int t = 0; t < thickness; t++) {
    for (int i = -width; i <= width; i++) {
      int x = DISPLAY_W / 2 + i;
      int y = MOUTH_Y + (i * i * depth) / (width * width) + t;
      display.drawPixel(x, y, GxEPD_BLACK);
    }
  }
}

static void drawFrown(int width, int depth, int thickness = 2) {
  for (int t = 0; t < thickness; t++) {
    for (int i = -width; i <= width; i++) {
      int x = DISPLAY_W / 2 + i;
      int y = MOUTH_Y + depth - (i * i * depth) / (width * width) + t;
      display.drawPixel(x, y, GxEPD_BLACK);
    }
  }
}

static void drawHappy() {
  display.fillScreen(GxEPD_WHITE);
  drawEyes(0, 0, 0, 0);
  drawSmile(20, 6, 2);
  display.drawCircle(LEFT_EYE_X - 12, EYE_Y + 15, 6, GxEPD_BLACK);
  display.drawCircle(RIGHT_EYE_X + 12, EYE_Y + 15, 6, GxEPD_BLACK);
}

static void drawSad() {
  display.fillScreen(GxEPD_WHITE);
  drawEyes(0, 3, 0, 3);
  display.drawLine(LEFT_EYE_X - 12, EYE_Y - 16, LEFT_EYE_X + 8, EYE_Y - 13, GxEPD_BLACK);
  display.drawLine(LEFT_EYE_X - 12, EYE_Y - 17, LEFT_EYE_X + 8, EYE_Y - 14, GxEPD_BLACK);
  display.drawLine(RIGHT_EYE_X - 8, EYE_Y - 13, RIGHT_EYE_X + 12, EYE_Y - 16, GxEPD_BLACK);
  display.drawLine(RIGHT_EYE_X - 8, EYE_Y - 14, RIGHT_EYE_X + 12, EYE_Y - 17, GxEPD_BLACK);
  drawFrown(18, 5, 2);
  display.fillCircle(LEFT_EYE_X + 14, EYE_Y + 12, 3, GxEPD_BLACK);
  display.fillTriangle(LEFT_EYE_X + 14, EYE_Y + 6,
                       LEFT_EYE_X + 11, EYE_Y + 12,
                       LEFT_EYE_X + 17, EYE_Y + 12, GxEPD_BLACK);
}

static void drawThinking() {
  display.fillScreen(GxEPD_WHITE);
  display.drawCircle(LEFT_EYE_X, EYE_Y, EYE_RADIUS, GxEPD_BLACK);
  display.drawCircle(LEFT_EYE_X, EYE_Y, EYE_RADIUS - 1, GxEPD_BLACK);
  display.fillCircle(LEFT_EYE_X - 2, EYE_Y - 3, PUPIL_RADIUS, GxEPD_BLACK);
  display.drawLine(RIGHT_EYE_X - 10, EYE_Y - 2, RIGHT_EYE_X + 10, EYE_Y - 2, GxEPD_BLACK);
  display.drawLine(RIGHT_EYE_X - 10, EYE_Y, RIGHT_EYE_X + 10, EYE_Y, GxEPD_BLACK);
  display.drawLine(RIGHT_EYE_X - 10, EYE_Y + 2, RIGHT_EYE_X + 10, EYE_Y + 2, GxEPD_BLACK);
  display.fillCircle(DISPLAY_W / 2 - 12, MOUTH_Y, 3, GxEPD_BLACK);
  display.fillCircle(DISPLAY_W / 2, MOUTH_Y, 3, GxEPD_BLACK);
  display.fillCircle(DISPLAY_W / 2 + 12, MOUTH_Y, 3, GxEPD_BLACK);
  display.drawCircle(RIGHT_EYE_X + 18, EYE_Y - 20, 2, GxEPD_BLACK);
  display.drawCircle(RIGHT_EYE_X + 24, EYE_Y - 28, 4, GxEPD_BLACK);
}

static void drawExcited() {
  display.fillScreen(GxEPD_WHITE);
  for (int i = -8; i <= 8; i++) {
    int y = EYE_Y - 4 + (i * i) / 10;
    display.drawPixel(LEFT_EYE_X + i, y, GxEPD_BLACK);
    display.drawPixel(LEFT_EYE_X + i, y + 1, GxEPD_BLACK);
  }
  for (int i = -8; i <= 8; i++) {
    int y = EYE_Y - 4 + (i * i) / 10;
    display.drawPixel(RIGHT_EYE_X + i, y, GxEPD_BLACK);
    display.drawPixel(RIGHT_EYE_X + i, y + 1, GxEPD_BLACK);
  }
  display.drawLine(LEFT_EYE_X - 18, EYE_Y - 3, LEFT_EYE_X - 24, EYE_Y - 3, GxEPD_BLACK);
  display.drawLine(LEFT_EYE_X - 21, EYE_Y - 8, LEFT_EYE_X - 21, EYE_Y + 2, GxEPD_BLACK);
  display.drawLine(RIGHT_EYE_X + 18, EYE_Y - 3, RIGHT_EYE_X + 24, EYE_Y - 3, GxEPD_BLACK);
  display.drawLine(RIGHT_EYE_X + 21, EYE_Y - 8, RIGHT_EYE_X + 21, EYE_Y + 2, GxEPD_BLACK);
  drawSmile(22, 10, 2);
  display.drawCircle(LEFT_EYE_X - 8, EYE_Y + 12, 5, GxEPD_BLACK);
  display.drawCircle(RIGHT_EYE_X + 8, EYE_Y + 12, 5, GxEPD_BLACK);
}

static void drawSleepy() {
  display.fillScreen(GxEPD_WHITE);
  display.drawLine(LEFT_EYE_X - 8, EYE_Y - 2, LEFT_EYE_X + 8, EYE_Y - 2, GxEPD_BLACK);
  display.drawLine(LEFT_EYE_X - 8, EYE_Y - 1, LEFT_EYE_X + 8, EYE_Y - 1, GxEPD_BLACK);
  display.drawLine(RIGHT_EYE_X - 8, EYE_Y - 2, RIGHT_EYE_X + 8, EYE_Y - 2, GxEPD_BLACK);
  display.drawLine(RIGHT_EYE_X - 8, EYE_Y - 1, RIGHT_EYE_X + 8, EYE_Y - 1, GxEPD_BLACK);
  display.drawCircle(DISPLAY_W / 2, MOUTH_Y, 8, GxEPD_BLACK);
  display.drawCircle(DISPLAY_W / 2, MOUTH_Y, 7, GxEPD_BLACK);
  display.fillRect(DISPLAY_W / 2 - 6, MOUTH_Y - 8, 12, 4, GxEPD_WHITE);
  display.fillRect(DISPLAY_W / 2 - 6, MOUTH_Y + 5, 12, 4, GxEPD_WHITE);
  display.setFont(&FreeMonoBold9pt7b);
  display.setCursor(80, 40);
  display.print("z");
  display.setCursor(92, 28);
  display.print("Z");
  display.setCursor(104, 16);
  display.print("Z");
}

static void drawCurious() {
  display.fillScreen(GxEPD_WHITE);
  display.drawCircle(LEFT_EYE_X, EYE_Y, EYE_RADIUS, GxEPD_BLACK);
  display.drawCircle(LEFT_EYE_X, EYE_Y, EYE_RADIUS - 1, GxEPD_BLACK);
  display.fillCircle(LEFT_EYE_X + 3, EYE_Y - 1, PUPIL_RADIUS, GxEPD_BLACK);
  display.drawCircle(RIGHT_EYE_X, EYE_Y, EYE_RADIUS + 2, GxEPD_BLACK);
  display.drawCircle(RIGHT_EYE_X, EYE_Y, EYE_RADIUS + 1, GxEPD_BLACK);
  display.fillCircle(RIGHT_EYE_X + 3, EYE_Y - 1, PUPIL_RADIUS, GxEPD_BLACK);
  display.drawLine(RIGHT_EYE_X - 14, EYE_Y - 18, RIGHT_EYE_X + 10, EYE_Y - 15, GxEPD_BLACK);
  display.drawLine(RIGHT_EYE_X - 14, EYE_Y - 19, RIGHT_EYE_X + 10, EYE_Y - 16, GxEPD_BLACK);
  display.drawCircle(DISPLAY_W / 2, MOUTH_Y, 6, GxEPD_BLACK);
  display.drawCircle(DISPLAY_W / 2, MOUTH_Y, 5, GxEPD_BLACK);
  display.setFont(&FreeMonoBold9pt7b);
  display.setCursor(5, 45);
  display.print("?");
}

static void showExpressionProgrammatic(const char* mood) {
  display.setFullWindow();
  display.firstPage();
  do {
    if (strcmp(mood, "happy") == 0) drawHappy();
    else if (strcmp(mood, "sad") == 0) drawSad();
    else if (strcmp(mood, "thinking") == 0) drawThinking();
    else if (strcmp(mood, "excited") == 0) drawExcited();
    else if (strcmp(mood, "sleepy") == 0) drawSleepy();
    else if (strcmp(mood, "curious") == 0) drawCurious();
    else drawHappy();

    display.setFont(&FreeMonoBold9pt7b);
    int16_t tbx, tby; uint16_t tbw, tbh;
    display.getTextBounds(mood, 0, 0, &tbx, &tby, &tbw, &tbh);
    display.setCursor((DISPLAY_W - tbw) / 2, 230);
    display.print(mood);
  } while (display.nextPage());
}

void displaySetSdAvailable(bool available) {
  sdCardAvailable = available;
}

void displayInit() {
  display.init(115200);
  display.setRotation(0);
  display.setTextColor(GxEPD_BLACK);
}

void displayShowExpression(const char* mood) {
  Serial.print("Showing expression: ");
  Serial.println(mood);

  if (sdCardAvailable && showExpressionFromSD(mood)) {
    Serial.println("Loaded from SD card");
  } else {
    Serial.println("Using programmatic drawing");
    showExpressionProgrammatic(mood);
  }
}

