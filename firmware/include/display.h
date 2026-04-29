#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include "sensor_hal.h"

void initDisplay();
void updateDisplay(const WeatherData& data, const char* prediction);

#endif
