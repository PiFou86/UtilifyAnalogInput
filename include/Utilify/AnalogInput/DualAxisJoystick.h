#pragma once

#include <Arduino.h>
#include <Utilify/AnalogInput/AnalogInput.h>
#include <Utilify/DigitalInput/PushButton.h>
#include <Utilify/Task/TaskBase.h>

/*
 * DualAxisJoystick class for reading analog values from two potentiometer pins.
 * It normalizes the values to a range of -512 to 511.
 * Inherits from AnalogInput.
 */
class DualAxisJoystickActionXChanged;
class DualAxisJoystickActionYChanged;
class DualAxisJoystick : public TaskBase {
 public:
  DualAxisJoystick(const uint8_t& pinX, const uint8_t& pinY,
                   const uint8_t& pinButton = 0,
                   ActionBase<float>* actionValueChangedX = nullptr,
                   ActionBase<float>* actionValueChangedY = nullptr,
                   ActionBase<void>* actionButtonPressed = nullptr,
                   ActionBase<void>* actionButtonReleased = nullptr,
                   bool invertX = false, bool invertY = false);
  DualAxisJoystick(const uint8_t& pinX, const uint8_t& pinY,
                   const uint8_t& pinButton,
                   CallbackWithParam<float> actionValueChangedX,
                   CallbackWithParam<float> actionValueChangedY,
                   Callback actionButtonPressed, Callback actionButtonReleased,
                   bool invertX = false, bool invertY = false);

  virtual void calibrate(Callback callbackCalibrating,
                         Callback callbackCalibrated);
  virtual void calibrate(ActionBase<void>* callbackCalibrating = nullptr,
                         ActionBase<void>* callbackCalibrated = nullptr);
  inline float valueX() const {
    float res =  (m_joystickX.value() > m_centerXValue)
               ? (float)((NORMALIZE_ANALOG_VALUE_TO_1023(m_joystickX.value()) -
                          m_centerXValue)) /
                     (m_maxXValue - m_centerXValue)
               : (m_centerXValue -
                  (float)NORMALIZE_ANALOG_VALUE_TO_1023(m_joystickX.value())) /
                     (m_minXValue - m_centerXValue);

    if (m_invertX) {
      res = -res;
    }
    return res;
  }
  inline float valueY() const {
    float res = (m_joystickY.value() > m_centerYValue)
               ? (float)((NORMALIZE_ANALOG_VALUE_TO_1023(m_joystickY.value()) -
                          m_centerYValue)) /
                     (m_maxYValue - m_centerYValue)
               : (m_centerYValue -
                  (float)NORMALIZE_ANALOG_VALUE_TO_1023(m_joystickY.value())) /
                     (m_minYValue - m_centerYValue);

    if (m_invertY) {
      res = -res;
    }
    return res;
  }

  inline void invertX(bool invert) { m_invertX = invert; }
  inline void invertY(bool invert) { m_invertY = invert; }

  inline void tick() override {
    m_joystickX.tick();
    m_joystickY.tick();
    m_button.tick();
  }

 private:
  AnalogInput m_joystickX;
  AnalogInput m_joystickY;
  PushButton m_button;
  int m_maxXValue;
  int m_maxYValue;
  int m_minXValue;
  int m_minYValue;
  int m_centerXValue;
  int m_centerYValue;
  ActionBase<float>* m_actionValueChangedX;
  ActionBase<float>* m_actionValueChangedY;
  CallbackWithParam<float> m_callbackValueChangedX;
  CallbackWithParam<float> m_callbackValueChangedY;

  bool m_calibrating = false;

  bool m_invertX = false;
  bool m_invertY = false;

  void _calibrate();

  friend class DualAxisJoystickActionXChanged;
  friend class DualAxisJoystickActionYChanged;
};
