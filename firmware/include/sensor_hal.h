#ifndef SENSOR_HAL_H
#define SENSOR_HAL_H

#include <Arduino.h>

struct WeatherData {
    float temperature;
    float humidity;
    float pressure;
    float light;
    bool valid;
};

void initSensors();
WeatherData readSensors();

#endif
