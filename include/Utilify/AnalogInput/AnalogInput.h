#pragma once

#include <Arduino.h>
#include <Utilify/Action/ActionBase.h>
#include <Utilify/Callback.h>
#include <Utilify/Task/TaskBase.h>

/*
  * AnalogInput class for reading analog values from a pin.
  * It supports value change detection with a tolerance and can notify
  * through an ActionBase or a CallbackWithParam.
  * Value is raw analog value, not normalized.
  * Use NORMALIZE_ANALOG_VALUE_TO_1023 to normalize
  */
class AnalogInput : public TaskBase {
 public:
  AnalogInput(const uint8_t& pin,
              ActionBase<int>* actionValueChanged = nullptr);
  AnalogInput(const uint8_t& pin, CallbackWithParam<int> actionValueChanged);
  ~AnalogInput() override = default;

  inline virtual int value() const { return m_value; }

  inline virtual void tolerance(const int& tolerance) { m_tolerance = tolerance; }
  inline virtual int tolerance() const { return m_tolerance; }

  void tick() override;
  inline virtual void setActionValueChanged(ActionBase<int>* actionValueChanged) {
    m_actionValueChanged = actionValueChanged;
  }
  inline
  virtual void setCallbackValueChanged(CallbackWithParam<int> callbackValueChanged) {
    m_callbackValueChanged = callbackValueChanged;
  }
 private:
  uint8_t m_pin;
  int m_value;
  int m_tolerance;
  unsigned long m_lastReadTime = 0;
  unsigned long m_readInterval = 5; // Default read interval in milliseconds
  ActionBase<int>* m_actionValueChanged = nullptr;
  CallbackWithParam<int> m_callbackValueChanged = nullptr;

  void notifyValueChanged();
};

// return max value of analog input
#ifdef ARDUINO_ARCH_AVR
#define MAX_ANALOG_VALUE 1023
#define NORMALIZE_ANALOG_VALUE_TO_1023(value) (value)
#elif defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_ESP8266)
#define MAX_ANALOG_VALUE 4095
#define NORMALIZE_ANALOG_VALUE_TO_1023(value) (value >> 2)
#else
#define MAX_ANALOG_VALUE 1023 // Default for other architectures
#define NORMALIZE_ANALOG_VALUE_TO_1023(value) (value)
#endif