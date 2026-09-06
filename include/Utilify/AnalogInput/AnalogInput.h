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

  inline virtual void tolerance(const int& tolerance) {
    if (tolerance >= 0) {
      m_tolerance = tolerance;
    }
  }
  inline virtual int tolerance() const { return m_tolerance; }

  void tick() override;
  inline virtual void setActionValueChanged(ActionBase<int>* actionValueChanged) {
    m_actionValueChanged = actionValueChanged;
    m_callbackValueChanged = nullptr;
  }
  inline
  virtual void setCallbackValueChanged(CallbackWithParam<int> callbackValueChanged) {
    m_callbackValueChanged = callbackValueChanged;
    m_actionValueChanged = nullptr;
  }

 protected:
  void notifyValueChanged();

 private:
  uint8_t m_pin;
  int m_value;
  int m_tolerance;
  unsigned long m_lastReadTime = 0;
  unsigned long m_readInterval = 5; // Default read interval in milliseconds
  ActionBase<int>* m_actionValueChanged = nullptr;
  CallbackWithParam<int> m_callbackValueChanged = nullptr;

};

// Resolution returned by analogRead(). Override this define in the build flags
// when the application configures another resolution.
#ifndef UTILIFY_ANALOG_INPUT_BITS
  #if defined(ARDUINO_ARCH_ESP32)
    #define UTILIFY_ANALOG_INPUT_BITS 12
  #else
    #define UTILIFY_ANALOG_INPUT_BITS 10
  #endif
#endif

#if UTILIFY_ANALOG_INPUT_BITS < 1 || UTILIFY_ANALOG_INPUT_BITS > 16
  #error "UTILIFY_ANALOG_INPUT_BITS must be between 1 and 16"
#endif

#define MAX_ANALOG_VALUE ((1UL << UTILIFY_ANALOG_INPUT_BITS) - 1UL)

#if UTILIFY_ANALOG_INPUT_BITS > 10
  #define NORMALIZE_ANALOG_VALUE_TO_1023(value) \
    ((value) >> (UTILIFY_ANALOG_INPUT_BITS - 10))
#elif UTILIFY_ANALOG_INPUT_BITS < 10
  #define NORMALIZE_ANALOG_VALUE_TO_1023(value) \
    ((value) << (10 - UTILIFY_ANALOG_INPUT_BITS))
#else
  #define NORMALIZE_ANALOG_VALUE_TO_1023(value) (value)
#endif
