#include <Arduino.h>
#include <Utilify/AnalogInput/DualAxisJoystick.h>
#include <Utilify/AnalogInput/Potentiometer.h>

class InputDisplay {
 public:
  void potentiometerChanged(const int& value) {
    Serial.print("Potentiometer: ");
    Serial.println(value);
  }

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
};

InputDisplay inputDisplay;
Potentiometer* potentiometer = nullptr;
DualAxisJoystick* joystick = nullptr;

#ifdef ARDUINO_ARCH_ESP32
constexpr uint8_t POTENTIOMETER_PIN = 34;
constexpr uint8_t JOYSTICK_X_PIN = 35;
constexpr uint8_t JOYSTICK_Y_PIN = 32;
constexpr uint8_t JOYSTICK_BUTTON_PIN = 27;
#else
constexpr uint8_t POTENTIOMETER_PIN = A0;
constexpr uint8_t JOYSTICK_X_PIN = A1;
constexpr uint8_t JOYSTICK_Y_PIN = A2;
constexpr uint8_t JOYSTICK_BUTTON_PIN = 2;
#endif

void setup() {
  Serial.begin(115200);
  analogReadResolution(UTILIFY_ANALOG_INPUT_BITS);
  InputDisplay* display = &inputDisplay;

  potentiometer = new Potentiometer(
      POTENTIOMETER_PIN,
      [display](const int& value) { display->potentiometerChanged(value); });
  potentiometer->tolerance(8);

  joystick = new DualAxisJoystick(
      JOYSTICK_X_PIN, JOYSTICK_Y_PIN, JOYSTICK_BUTTON_PIN,
      [display](const float& value) { display->xChanged(value); },
      [display](const float& value) { display->yChanged(value); },
      [display]() { display->buttonPressed(); },
      [display]() { display->buttonReleased(); });

  joystick->calibrate(
      []() { Serial.println("Move the joystick for 10 seconds"); },
      []() { Serial.println("Calibration complete"); });
}

void loop() {
  potentiometer->tick();
  joystick->tick();
}
