# UtilifyAnalogInput

**UtilifyAnalogInput** is a library that provides a versatile collection of analog inputs for **Arduino** and **ESP32** platforms. Designed to simplify sensor management, it offers reliable detection of value changes and integrates seamlessly with the **Utilify** library.

This library includes support for various analog input devices, such as potentiometers and dual-axis joysticks, allowing developers to easily read and respond to analog signals in their embedded applications.

[![PlatformIO Registry](https://badges.registry.platformio.org/packages/sirdrako/library/UtilifyAnalogInput.svg)](https://registry.platformio.org/libraries/sirdrako/UtilifyAnalogInput)

## Features

- **Analog Input Management**: Easily manage multiple analog inputs with configurable thresholds and callbacks.
- **Potentiometer Support**: Includes a Potentiometer class for reading and interpreting analog values.
- **DualAxisJoystick Support**: Provides a DualAxisJoystick class for reading joystick movements.
- **Wide Compatibility**: Works on **Arduino** (Atmel AVR) and **ESP32** as well as other supported platforms.
- **Easy Integration**: Easily integrates into your PlatformIO projects.

## Getting Started

### Installation

To add **UtilifyAnalogInput** to your PlatformIO project, include the library in your `platformio.ini` file:

```ini
[env:uno]
platform = atmelavr
board = uno
monitor_speed = 9600
lib_deps =
    https://github.com/PiFou86/Utilify.git
    https://github.com/PiFou86/UtilifyDigitalInput.git
    https://github.com/PiFou86/UtilifyAnalogInput.git

[env:esp32doit-devkit-v1]
platform = espressif32
board = esp32doit-devkit-v1
monitor_speed = 115200
lib_deps =
    https://github.com/PiFou86/Utilify.git
    https://github.com/PiFou86/UtilifyDigitalInput.git
    https://github.com/PiFou86/UtilifyAnalogInput.git
monitor_filters = esp32_exception_decoder
```

### Basic Usage

Include the library in your project and set up your callbacks to respond to various analog input events:

```cpp
#include <Arduino.h>
#include <Utilify/AnalogInput/AnalogInput.h>
#include <Utilify/AnalogInput/Potentiometer.h>

#include <Utilify/AnalogInput/DualAxisJoystick.h>

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
```

When you run this code, it initializes either a potentiometer or a joystick, depending on which section is uncommented. The values from the analog inputs are printed to the Serial Monitor whenever they change beyond the defined tolerance.

## Feature Overview

### Analog Input Management

The library provides a unified interface for managing various analog input devices, including potentiometers and joysticks. You can easily create instances of these devices and set up callbacks to respond to value changes.

### Integration with Utilify

UtilifyAnalogInput depends on the Utilify and UtilifyDigitalInput libraries, ensuring seamless integration with other utilities and facilitating the development of robust embedded applications.

## Dependencies

The library depends on the following:

- [Utilify](https://github.com/PiFou86/Utilify): Core utilities for string manipulation, timer and device-specific operations.
- [UtilifyDigitalInput](https://github.com/PiFou86/UtilifyDigitalInput): Utilities for managing digital input devices, including buttons and switches.

## Author

**Pierre-François Léon @PiFou86** 
🔗 [Linkedin](https://www.linkedin.com/in/pierrefrancoisleon/)  
🔗 [GitHub Profile](https://github.com/PiFou86)
