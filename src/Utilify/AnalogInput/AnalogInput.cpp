#include <Utilify/AnalogInput/AnalogInput.h>

AnalogInput::AnalogInput(const uint8_t& pin,
                         ActionBase<int>* actionValueChanged)
    : m_pin(pin),
      m_value(0),
      m_tolerance(2),
      m_lastReadTime(0),
      m_actionValueChanged(actionValueChanged) {
  pinMode(m_pin, INPUT);
  this->m_value = analogRead(m_pin);
  this->m_lastReadTime = millis();
  notifyValueChanged();
}

AnalogInput::AnalogInput(const uint8_t& pin,
                         CallbackWithParam<int> actionValueChanged)
    : m_pin(pin),
      m_value(0),
      m_tolerance(2),
      m_lastReadTime(0),
      m_callbackValueChanged(actionValueChanged) {
  pinMode(m_pin, INPUT);
  this->m_value = analogRead(m_pin);
  this->m_lastReadTime = millis();
  notifyValueChanged();
}

void AnalogInput::tick() {
  if (millis() - m_lastReadTime < m_readInterval) {
    return;
  }

  m_lastReadTime = millis();
  int newValue = analogRead(m_pin);
  if (abs(newValue - m_value) > m_tolerance) {
    m_value = newValue;
    notifyValueChanged();
  }
}

void AnalogInput::notifyValueChanged() {
  if (m_actionValueChanged) {
    m_actionValueChanged->execute(m_value);
  } else if (m_callbackValueChanged) {
    m_callbackValueChanged(m_value);
  }
}
