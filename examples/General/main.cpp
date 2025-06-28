#include <Arduino.h>

#include <Utilify/Action/ActionBase.h>

#include <Utilify/AnalogInput/AnalogInput.h>
#include <Utilify/AnalogInput/Potentiometer.h>

#include <Utilify/AnalogInput/DualAxisJoystick.h>

#ifdef ESP32
#define A0 36
#define A1 39
#else
#define A0 A0
#define A1 A1
#endif

//#define POTENTIOMETER
#define JOYSTICK

#ifdef POTENTIOMETER
AnalogInput* potentiometerVert = nullptr;
AnalogInput* potentiometerHorz = nullptr;
#endif

#ifdef JOYSTICK
DualAxisJoystick* joystick = nullptr;
#endif

void setup() {
#ifdef ARDUINO_ARCH_AVR
    Serial.begin(9600);
#else
    Serial.begin(115200);
#endif
    while (!Serial) {
        ; // Wait for Serial to be ready
    }

#ifdef POTENTIOMETER
    Serial.println("Initializing potentiometers...");
    potentiometerVert = new AnalogInput(A0, [](const int& value) {
        Serial.print("Potentiometer vertical value: ");
        Serial.println(value);
    });
     potentiometerVert->tolerance(5); // Set tolerance for value change detection

    potentiometerHorz = new AnalogInput(A1, [](const int& value) {
        Serial.print("Potentiometer horizontal value: ");
        Serial.println(value);
    }); 
    potentiometerHorz->tolerance(5); // Set tolerance for value change detection
#endif

#ifdef JOYSTICK
    Serial.println("Initializing joystick...");
    joystick = new DualAxisJoystick(A1, A0, 2,
        [](const float& valueX) {
            Serial.print("Joystick X value: ");
            Serial.println(valueX);
        },
        [](const float& valueY) {
            Serial.print("Joystick Y value: ");
            Serial.println(valueY);
        },
        []() {
            Serial.println("Joystick button pressed");
        },
        []() {
            Serial.println("Joystick button released");
        }, true, false); // Invert X axis, do not invert Y axis

    Serial.println("Joystick initialized.");
#endif
}

bool isCalibrated = false;
void loop() {
#ifdef POTENTIOMETER
    potentiometerVert->tick();
    potentiometerHorz->tick();
#endif

#ifdef JOYSTICK
    if (!isCalibrated) {
        joystick->calibrate([]() {
            Serial.println("Calibrating joystick...");
        }, []() {
            Serial.println("Joystick calibration complete.");
            isCalibrated = true;
        });
    }
   joystick->tick();
#endif
}