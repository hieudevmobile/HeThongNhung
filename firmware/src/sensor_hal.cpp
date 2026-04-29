#include "sensor_hal.h"
#include <DHT.h>
#include <Adafruit_BMP085.h>
#include <BH1750.h>
#include <Wire.h>

#define DHTPIN D3
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
Adafruit_BMP085 bmp;
BH1750 lightMeter;

void initSensors() {
    Wire.begin();
    dht.begin();
    Serial.println("--- Sensor Initialization ---");
    
    // DHT Check
    float t = dht.readTemperature();
    if (isnan(t)) {
        Serial.println("[FAIL] DHT11 not responding!");
    } else {
        Serial.println("[OK] DHT11 initialized.");
    }

    if (!bmp.begin()) {
        Serial.println("[FAIL] BMP180 not found (Check SDA/SCL/Power)!");
    } else {
        Serial.println("[OK] BMP180 initialized.");
    }

    if (!lightMeter.begin()) {
        Serial.println("[FAIL] BH1750 not found!");
    } else {
        Serial.println("[OK] BH1750 initialized.");
    }
    Serial.println("-----------------------------");
}

WeatherData readSensors() {
    WeatherData data;
    data.temperature = dht.readTemperature();
    data.humidity = dht.readHumidity();
    data.pressure = bmp.readPressure() / 100.0F; // hPa
    data.light = lightMeter.readLightLevel();

    // === DEBUG: Offset để test ===
    // Bỏ comment để tăng nhiệt độ
    // data.temperature += 5.0;  // +5°C
    
    // Bỏ comment để tăng độ ẩm
    // data.humidity += 15.0;  // +15%
    
    // Bỏ comment để tăng áp suất
    // data.pressure += 10.0;  // +10 hPa
    
    // Bỏ comment để tăng ánh sáng
    // data.light += 200.0;  // +200 Lux
    // ==========================

    if (isnan(data.temperature) || isnan(data.humidity) || data.pressure == 0) {
        data.valid = false;
    } else {
        data.valid = true;
    }
    return data;
}
