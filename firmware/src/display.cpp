#include "display.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void initDisplay() {
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("SSD1306 allocation failed"));
    }
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(0,0);
    display.println("Weather Station Init...");
    display.display();
}

void updateDisplay(const WeatherData& data, const char* prediction) {
    display.clearDisplay();
    
    // Header
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println(" WEATHER STATION ML ");
    display.drawFastHLine(0, 10, 128, SSD1306_WHITE);

    // Data Rows
    display.setCursor(0, 15);
    display.print("Temperature:");
    display.setCursor(80, 15);
    display.print(data.temperature, 1);
    display.print(" C");

    display.setCursor(0, 25);
    display.print("Humidity:");
    display.setCursor(80, 25);
    display.print(data.humidity, 1);
    display.print(" %");

    display.setCursor(0, 35);
    display.print("Pressure:");
    display.setCursor(80, 35);
    display.print(data.pressure, 0);
    display.print(" hP");

    display.setCursor(0, 45);
    display.print("Light:");
    display.setCursor(80, 45);
    display.print(data.light, 0);
    display.print(" Lx");

    // Divider
    display.drawFastHLine(0, 54, 128, SSD1306_WHITE);

    // Prediction Area (Centered & Large)
    display.setTextSize(2);
    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(prediction, 0, 0, &x1, &y1, &w, &h);
    display.setCursor((128 - w) / 2, 57);
    display.println(prediction);
    
    display.display();
}
