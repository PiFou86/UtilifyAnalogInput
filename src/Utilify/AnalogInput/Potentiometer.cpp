#include <Utilify/AnalogInput/Potentiometer.h>

Potentiometer::Potentiometer(int pin, ActionBase<int>* actionValueChanged)
    : AnalogInput(pin) {
  setActionValueChanged(actionValueChanged);
  notifyValueChanged();
}

Potentiometer::Potentiometer(int pin, CallbackWithParam<int> actionValueChanged)
    : AnalogInput(pin) {
  setCallbackValueChanged(actionValueChanged);
  notifyValueChanged();
}
