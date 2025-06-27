#pragma once

#include "AnalogInput.h"

/*
  * Potentiometer class for reading analog values from a potentiometer pin.
  * It normalizes the value to a range of 0-1023.
  * Inherits from AnalogInput.
*/
class Potentiometer : public AnalogInput {
 public:
  Potentiometer(int pin, ActionBase<int>* actionValueChanged = nullptr);
  Potentiometer(int pin, CallbackWithParam<int> actionValueChanged);

  inline int value() const override {
    return NORMALIZE_ANALOG_VALUE_TO_1023(AnalogInput::value());
  }
};
