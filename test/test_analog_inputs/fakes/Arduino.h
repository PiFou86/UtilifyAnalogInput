#pragma once

#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#define HIGH 0x1
#define LOW 0x0
#define INPUT 0x0
#define OUTPUT 0x1
#define INPUT_PULLUP 0x2

#define A0 0
#define A1 1

class FakeSerialClass {
 public:
  template <typename T>
  void print(const T&) {}

  template <typename T>
  void println(const T&) {}

  void println() {}
};

extern FakeSerialClass Serial;

unsigned long millis();
int analogRead(uint8_t pin);
int digitalRead(uint8_t pin);
void digitalWrite(uint8_t pin, uint8_t value);
void pinMode(uint8_t pin, uint8_t mode);
