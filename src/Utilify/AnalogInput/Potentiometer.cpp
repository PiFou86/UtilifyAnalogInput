#include <Utilify/AnalogInput/Potentiometer.h>

Potentiometer::Potentiometer(int pin, ActionBase<int>* actionValueChanged)
    : AnalogInput(pin, actionValueChanged) {;}

Potentiometer::Potentiometer(int pin, CallbackWithParam<int> actionValueChanged)
    : AnalogInput(pin, actionValueChanged) {;}
