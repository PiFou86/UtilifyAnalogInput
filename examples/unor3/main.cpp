#include <Arduino.h>
#include <Utilify/AnalogInput/DualAxisJoystick.h>
#include <Utilify/AnalogInput/Potentiometer.h>

class PotentiometerPrinter : public ActionBase<int> {
 public:
  void execute(const int& value) override {
    Serial.print("Potentiometer: ");
    Serial.println(value);
  }
};

void xChanged(const float& value) {
  Serial.print("Joystick X: ");
  Serial.println(value);
}

void yChanged(const float& value) {
  Serial.print("Joystick Y: ");
  Serial.println(value);
}

void buttonPressed() { Serial.println("Joystick button pressed"); }
void buttonReleased() { Serial.println("Joystick button released"); }
void calibrationStarted() { Serial.println("Move the joystick for 10 seconds"); }
void calibrationFinished() { Serial.println("Calibration complete"); }

PotentiometerPrinter potentiometerPrinter;
Potentiometer* potentiometer = nullptr;
DualAxisJoystick* joystick = nullptr;

void setup() {
  Serial.begin(9600);

  potentiometer = new Potentiometer(A0, &potentiometerPrinter);
  potentiometer->tolerance(4);

  joystick = new DualAxisJoystick(A1, A2, 2, xChanged, yChanged,
                                  buttonPressed, buttonReleased);
  joystick->calibrate(calibrationStarted, calibrationFinished);
}

void loop() {
  potentiometer->tick();
  joystick->tick();
}
