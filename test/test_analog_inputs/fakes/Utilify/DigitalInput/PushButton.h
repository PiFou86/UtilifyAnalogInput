#pragma once

#include <Arduino.h>
#include <Utilify/Action/ActionBase.h>
#include <Utilify/Callback.h>
#include <Utilify/Task/TaskBase.h>

class PushButton : public TaskBase {
 public:
  PushButton(uint8_t pin, ActionBase<void>* actionKeyUp = nullptr,
             ActionBase<void>* actionKeyDown = nullptr,
             ActionBase<void>* = nullptr, ActionBase<void>* = nullptr,
             const unsigned int& = 3000)
      : m_pin(pin),
        m_lastState(HIGH),
        m_actionKeyUp(actionKeyUp),
        m_actionKeyDown(actionKeyDown) {
    pinMode(m_pin, INPUT_PULLUP);
  }

  PushButton(uint8_t pin, Callback callbackKeyUp,
             Callback callbackKeyDown = nullptr,
             Callback = nullptr, Callback = nullptr,
             const unsigned int& = 3000)
      : m_pin(pin),
        m_lastState(HIGH),
        m_callbackKeyUp(callbackKeyUp),
        m_callbackKeyDown(callbackKeyDown) {
    pinMode(m_pin, INPUT_PULLUP);
  }

  void callbackKeyDown(ActionBase<void>* action) {
    m_actionKeyDown = action;
    m_callbackKeyDown = nullptr;
  }

  void callbackKeyDown(Callback callback) {
    m_callbackKeyDown = callback;
    m_actionKeyDown = nullptr;
  }

  void tick() override {
    const int state = digitalRead(m_pin);
    if (state == m_lastState) {
      return;
    }

    m_lastState = state;
    if (state == LOW) {
      if (m_actionKeyDown) {
        m_actionKeyDown->execute();
      } else if (m_callbackKeyDown) {
        m_callbackKeyDown();
      }
    } else if (m_actionKeyUp) {
      m_actionKeyUp->execute();
    } else if (m_callbackKeyUp) {
      m_callbackKeyUp();
    }
  }

 private:
  uint8_t m_pin;
  int m_lastState;
  ActionBase<void>* m_actionKeyUp = nullptr;
  ActionBase<void>* m_actionKeyDown = nullptr;
  Callback m_callbackKeyUp = nullptr;
  Callback m_callbackKeyDown = nullptr;
};
