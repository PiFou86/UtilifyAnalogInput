#include "FakeArduino.h"

FakeSerialClass Serial;

namespace {

unsigned long currentMillis;
unsigned long millisAutoAdvance;
int analogValues[256];
int digitalValues[256];
uint8_t pinModes[256];
bool alternatingAnalogValues[256];
bool alternatingState[256];
int alternatingFirst[256];
int alternatingSecond[256];

}  // namespace

namespace FakeArduino {

void reset() {
  currentMillis = 0;
  millisAutoAdvance = 0;

  for (size_t pin = 0; pin < 256; ++pin) {
    analogValues[pin] = 0;
    digitalValues[pin] = HIGH;
    pinModes[pin] = INPUT;
    alternatingAnalogValues[pin] = false;
    alternatingState[pin] = false;
    alternatingFirst[pin] = 0;
    alternatingSecond[pin] = 0;
  }
}

void setMillis(unsigned long value) { currentMillis = value; }

void advanceMillis(unsigned long duration) { currentMillis += duration; }

void setMillisAutoAdvance(unsigned long duration) {
  millisAutoAdvance = duration;
}

void setAnalogValue(uint8_t pin, int value) { analogValues[pin] = value; }

void setAlternatingAnalogValues(uint8_t pin, int first, int second) {
  alternatingAnalogValues[pin] = true;
  alternatingState[pin] = false;
  alternatingFirst[pin] = first;
  alternatingSecond[pin] = second;
}

void clearAlternatingAnalogValues(uint8_t pin) {
  alternatingAnalogValues[pin] = false;
}

void setDigitalValue(uint8_t pin, int value) { digitalValues[pin] = value; }

uint8_t pinModeValue(uint8_t pin) { return pinModes[pin]; }

}  // namespace FakeArduino

unsigned long millis() {
  const unsigned long result = currentMillis;
  currentMillis += millisAutoAdvance;
  return result;
}

int analogRead(uint8_t pin) {
  if (!alternatingAnalogValues[pin]) {
    return analogValues[pin];
  }

  alternatingState[pin] = !alternatingState[pin];
  return alternatingState[pin] ? alternatingFirst[pin]
                               : alternatingSecond[pin];
}

int digitalRead(uint8_t pin) { return digitalValues[pin]; }

void digitalWrite(uint8_t pin, uint8_t value) { digitalValues[pin] = value; }

void pinMode(uint8_t pin, uint8_t mode) { pinModes[pin] = mode; }
