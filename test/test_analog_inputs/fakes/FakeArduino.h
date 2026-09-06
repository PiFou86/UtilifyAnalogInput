#pragma once

#include <Arduino.h>

namespace FakeArduino {

void reset();
void setMillis(unsigned long value);
void advanceMillis(unsigned long duration);
void setMillisAutoAdvance(unsigned long duration);

void setAnalogValue(uint8_t pin, int value);
void setAlternatingAnalogValues(uint8_t pin, int first, int second);
void clearAlternatingAnalogValues(uint8_t pin);

void setDigitalValue(uint8_t pin, int value);
uint8_t pinModeValue(uint8_t pin);

}  // namespace FakeArduino
