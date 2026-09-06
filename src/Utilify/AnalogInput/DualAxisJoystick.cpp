#include <Utilify/AnalogInput/DualAxisJoystick.h>

DualAxisJoystickActionXChanged::DualAxisJoystickActionXChanged(
    DualAxisJoystick& joystick)
    : m_joystick(joystick) {}

void DualAxisJoystickActionXChanged::execute(const int&) {
  if (m_joystick.m_calibrating) {
    return;
  }
  if (m_joystick.m_actionValueChangedX) {
    m_joystick.m_actionValueChangedX->execute(m_joystick.valueX());
  } else if (m_joystick.m_callbackValueChangedX) {
    m_joystick.m_callbackValueChangedX(m_joystick.valueX());
  }
}

DualAxisJoystickActionYChanged::DualAxisJoystickActionYChanged(
    DualAxisJoystick& joystick)
    : m_joystick(joystick) {}

void DualAxisJoystickActionYChanged::execute(const int&) {
  if (m_joystick.m_calibrating) {
    return;
  }
  if (m_joystick.m_actionValueChangedY) {
    m_joystick.m_actionValueChangedY->execute(m_joystick.valueY());
  } else if (m_joystick.m_callbackValueChangedY) {
    m_joystick.m_callbackValueChangedY(m_joystick.valueY());
  }
}

DualAxisJoystick::DualAxisJoystick(const uint8_t& pinX, const uint8_t& pinY,
                                   const uint8_t& pinButton,
                                   ActionBase<float>* actionValueChangedX,
                                   ActionBase<float>* actionValueChangedY,
                                   ActionBase<void>* actionButtonPressed,
                                   ActionBase<void>* actionButtonReleased,
                                   bool invertX, bool invertY)
    : m_joystickX(pinX),
      m_joystickY(pinY),
      m_button(pinButton, actionButtonReleased),
      m_actionXChanged(*this),
      m_actionYChanged(*this),
      m_maxXValue(1023),
      m_maxYValue(1023),
      m_minXValue(0),
      m_minYValue(0),
      m_centerXValue(512),
      m_centerYValue(512),
      m_actionValueChangedX(actionValueChangedX),
      m_actionValueChangedY(actionValueChangedY),
      m_callbackValueChangedX(nullptr),
      m_callbackValueChangedY(nullptr),
      m_calibrating(false),
      m_invertX(invertX),
      m_invertY(invertY) {
  m_button.callbackKeyDown(actionButtonPressed);
  m_joystickX.setActionValueChanged(&m_actionXChanged);
  m_joystickY.setActionValueChanged(&m_actionYChanged);
  m_joystickX.tolerance(1);
  m_joystickY.tolerance(1);
}

DualAxisJoystick::DualAxisJoystick(
    const uint8_t& pinX, const uint8_t& pinY, const uint8_t& pinButton,
    CallbackWithParam<float> callbackValueChangedX,
    CallbackWithParam<float> callbackValueChangedY,
    Callback callbackButtonPressed, Callback callbackButtonReleased,
    bool invertX, bool invertY)
    : m_joystickX(pinX),
      m_joystickY(pinY),
      m_button(pinButton, callbackButtonReleased),
      m_actionXChanged(*this),
      m_actionYChanged(*this),
      m_maxXValue(1023),
      m_maxYValue(1023),
      m_minXValue(0),
      m_minYValue(0),
      m_centerXValue(512),
      m_centerYValue(512),
      m_actionValueChangedX(nullptr),
      m_actionValueChangedY(nullptr),
      m_callbackValueChangedX(callbackValueChangedX),
      m_callbackValueChangedY(callbackValueChangedY),
      m_calibrating(false),
      m_invertX(invertX),
      m_invertY(invertY) {
  m_button.callbackKeyDown(callbackButtonPressed);
  m_joystickX.setActionValueChanged(&m_actionXChanged);
  m_joystickY.setActionValueChanged(&m_actionYChanged);
  m_joystickX.tolerance(1);
  m_joystickY.tolerance(1);
}

void DualAxisJoystick::calibrate(Callback callbackCalibrating,
                                 Callback callbackCalibrated) {
  if (callbackCalibrating) {
    callbackCalibrating();
  }

  this->_calibrate();

  if (callbackCalibrated) {
    callbackCalibrated();
  }
}

void DualAxisJoystick::calibrate(ActionBase<void>* callbackCalibrating,
                                 ActionBase<void>* callbackCalibrated) {
  if (callbackCalibrating) {
    callbackCalibrating->execute();
  }

  this->_calibrate();

  if (callbackCalibrated) {
    callbackCalibrated->execute();
  }
}

void DualAxisJoystick::_calibrate() {
  this->m_centerXValue =
      NORMALIZE_ANALOG_VALUE_TO_1023(m_joystickX.value());
  this->m_centerYValue =
      NORMALIZE_ANALOG_VALUE_TO_1023(m_joystickY.value());
  this->m_maxXValue = m_centerXValue;
  this->m_maxYValue = m_centerYValue;
  this->m_minXValue = m_centerXValue;
  this->m_minYValue = m_centerYValue;

  unsigned long startTime = millis();

  this->m_calibrating = true;

  int xvalue;
  int yvalue;
  while (millis() - startTime < 10000) {
    m_joystickX.tick();
    m_joystickY.tick();

    xvalue = NORMALIZE_ANALOG_VALUE_TO_1023(m_joystickX.value());
    yvalue = NORMALIZE_ANALOG_VALUE_TO_1023(m_joystickY.value());

    if (xvalue > m_maxXValue) {
      m_maxXValue = xvalue;
    }
    if (yvalue > m_maxYValue) {
      m_maxYValue = yvalue;
    }
    if (xvalue < m_minXValue) {
      m_minXValue = xvalue;
    }
    if (yvalue < m_minYValue) {
      m_minYValue = yvalue;
    }
  }

  this->m_calibrating = false;

  Serial.print("DualAxisJoystick calibrated: ");
  Serial.print("X: ");
  Serial.print(m_centerXValue);
  Serial.print(", Y: ");
  Serial.println(m_centerYValue);
  Serial.print("Max X: ");
  Serial.print(m_maxXValue);
  Serial.print(", Min X: ");
  Serial.print(m_minXValue);
  Serial.print(", Max Y: ");
  Serial.print(m_maxYValue);
  Serial.print(", Min Y: ");
  Serial.println(m_minYValue);
  Serial.println("Calibration complete.");
}
