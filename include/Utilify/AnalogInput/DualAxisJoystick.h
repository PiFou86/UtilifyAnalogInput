#pragma once

#include <Arduino.h>
#include <Utilify/AnalogInput/AnalogInput.h>
#include <Utilify/DigitalInput/PushButton.h>
#include <Utilify/Task/TaskBase.h>

/*
 * DualAxisJoystick class for reading analog values from two potentiometer pins.
 * It normalizes each axis to a range of approximately -1.0 to 1.0.
 */
class DualAxisJoystick;

class DualAxisJoystickActionXChanged : public ActionBase<int> {
 public:
  explicit DualAxisJoystickActionXChanged(DualAxisJoystick& joystick);
  void execute(const int&) override;

 private:
  DualAxisJoystick& m_joystick;
};

class DualAxisJoystickActionYChanged : public ActionBase<int> {
 public:
  explicit DualAxisJoystickActionYChanged(DualAxisJoystick& joystick);
  void execute(const int&) override;

 private:
  DualAxisJoystick& m_joystick;
};

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
    const int value = NORMALIZE_ANALOG_VALUE_TO_1023(m_joystickX.value());
    float res = 0.0F;
    if (value > m_centerXValue && m_maxXValue > m_centerXValue) {
      res = (float)(value - m_centerXValue) /
            (m_maxXValue - m_centerXValue);
    } else if (value < m_centerXValue && m_minXValue < m_centerXValue) {
      res = (float)(m_centerXValue - value) /
            (m_minXValue - m_centerXValue);
    }

    if (m_invertX) {
      res = -res;
    }
    return res;
  }
  inline float valueY() const {
    const int value = NORMALIZE_ANALOG_VALUE_TO_1023(m_joystickY.value());
    float res = 0.0F;
    if (value > m_centerYValue && m_maxYValue > m_centerYValue) {
      res = (float)(value - m_centerYValue) /
            (m_maxYValue - m_centerYValue);
    } else if (value < m_centerYValue && m_minYValue < m_centerYValue) {
      res = (float)(m_centerYValue - value) /
            (m_minYValue - m_centerYValue);
    }

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
  DualAxisJoystickActionXChanged m_actionXChanged;
  DualAxisJoystickActionYChanged m_actionYChanged;
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
