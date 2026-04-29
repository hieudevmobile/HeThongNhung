#include <Arduino.h>
#include "sensor_hal.h"
#include "display.h"
#include "model.h"

// Prediction labels
const char* weather_labels[] = {"Sunny", "Rain", "Cloudy"};

// Simple smoothing: Moving average or basic filtering
int last_predictions[5];
int prediction_index = 0;

int get_smoothed_prediction(int new_pred) {
    last_predictions[prediction_index] = new_pred;
    prediction_index = (prediction_index + 1) % 5;
    
    // Simple majority vote
    int counts[3] = {0, 0, 0};
    for(int i = 0; i < 5; i++) {
        counts[last_predictions[i]]++;
    }
    
    if (counts[0] >= counts[1] && counts[0] >= counts[2]) return 0;
    if (counts[1] >= counts[0] && counts[1] >= counts[2]) return 1;
    return 2;
}

void setup() {
    Serial.begin(115200);
    Serial.println("\n--- ESP8266 Weather Station ---");
    
    initSensors();
    initDisplay();
    
    // Initialize smoothing buffer
    for(int i = 0; i < 5; i++) last_predictions[i] = 0;
}

void loop() {
    WeatherData data = readSensors();
    
    if (data.valid) {
        // Dự đoán dựa trên ML model Random Forest với 4 features
        double features[4] = {
            (double)data.temperature,
            (double)data.humidity,
            (double)data.pressure,
            (double)data.light
        };
        int raw_pred = predict(features);
        int smoothed_pred = get_smoothed_prediction(raw_pred);
        
        const char* result_label = weather_labels[smoothed_pred];
        
        // Serial Logging
        Serial.printf("T: %.1f, H: %.1f, P: %.1f, L: %.1f -> Prediction: %s\n", 
                      data.temperature, data.humidity, data.pressure, data.light, result_label);
        
        // Update OLED
        updateDisplay(data, result_label);
    } else {
        Serial.println("Sensor error or invalid data!");
        // Display Error
        updateDisplay(data, "Sensor Error");
    }
    
    delay(1000); // Wait 1 second - faster updates for testing
}
