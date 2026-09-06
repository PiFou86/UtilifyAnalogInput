# UtilifyAnalogInput

**Documentation:**
[English](https://github.com/PiFou86/UtilifyAnalogInput/blob/main/docs/en/README.md) |
[Français](https://github.com/PiFou86/UtilifyAnalogInput/blob/main/docs/fr/README.md)

UtilifyAnalogInput provides cooperative analog-input handling for Arduino Uno
R3, Arduino Uno R4, and ESP32. It includes raw analog inputs, normalized
potentiometers, and calibrated two-axis joysticks with callbacks or Utilify
actions.

[![CI](https://github.com/PiFou86/UtilifyAnalogInput/actions/workflows/ci.yml/badge.svg)](https://github.com/PiFou86/UtilifyAnalogInput/actions/workflows/ci.yml)
[![PlatformIO Registry](https://badges.registry.platformio.org/packages/sirdrako/library/UtilifyAnalogInput.svg)](https://registry.platformio.org/libraries/sirdrako/UtilifyAnalogInput)

## Features

- Raw `AnalogInput` readings with tolerance-based change detection.
- `Potentiometer` values normalized to a 10-bit domain.
- `DualAxisJoystick` values normalized to approximately `-1.0` through `1.0`.
- Joystick calibration, axis inversion, and push-button events.
- Function callbacks and non-owning `ActionBase` objects.
- Compile-time ADC resolution conversion with no runtime configuration lookup.
- Cooperative polling without interrupts or background threads.

## Installation

### PlatformIO

```ini
[env:unor3]
platform = atmelavr
board = uno
framework = arduino
lib_deps =
    sirdrako/UtilifyAnalogInput
```

PlatformIO installs the declared Utilify and UtilifyDigitalInput dependencies.

### Arduino IDE

Install UtilifyAnalogInput, UtilifyDigitalInput, and Utilify with Library
Manager. Alternatively, place all three repositories in the sketchbook
`libraries` directory and restart the IDE.

## Quick start

```cpp
#include <Arduino.h>
#include <Utilify/AnalogInput/Potentiometer.h>

void valueChanged(const int& value) {
  Serial.println(value);
}

Potentiometer* potentiometer = nullptr;

void setup() {
  Serial.begin(9600);
  potentiometer = new Potentiometer(A0, valueChanged);
  potentiometer->tolerance(4);
}

void loop() {
  potentiometer->tick();
}
```

The callback is also invoked once by the constructor with the initial reading.
Call `tick()` frequently; the library samples each analog input at most once
every 5 ms.

## ADC resolution

`AnalogInput` exposes the raw `analogRead()` result. `Potentiometer` and
`DualAxisJoystick` convert that result to a 10-bit domain. Defaults are 10 bits
on Arduino boards and 12 bits on ESP32.

If an application changes the returned resolution, configure both sides
with the same value:

```ini
build_flags =
    -D UTILIFY_ANALOG_INPUT_BITS=11
```

```cpp
void setup() {
  analogReadResolution(UTILIFY_ANALOG_INPUT_BITS);
}
```

Values from 1 through 16 are accepted. Conversion uses compile-time shifts, so
it adds no runtime resolution detection. Below 10 bits, the normalized maximum
is slightly below 1023: 9-bit input reaches 1022, for example.

## Supported platforms

| Capability | Uno R3 (AVR) | Uno R4 Minima/WiFi | ESP32 |
|---|---|---|---|
| Default returned ADC resolution | 10 bits | 10 bits | 12 bits |
| Free functions and `static` methods | Yes | Yes | Yes |
| Non-capturing lambdas | Yes | Yes | Yes |
| Capturing lambdas | No | Yes | Yes |
| `ActionBase<T>` | Yes | Yes | Yes |

## Documentation, examples, and tests

- [English documentation](https://github.com/PiFou86/UtilifyAnalogInput/blob/main/docs/en/README.md)
- [Documentation française](https://github.com/PiFou86/UtilifyAnalogInput/blob/main/docs/fr/README.md)
- [Uno R3 example](https://github.com/PiFou86/UtilifyAnalogInput/blob/main/examples/unor3/main.cpp)
- [ESP32 example](https://github.com/PiFou86/UtilifyAnalogInput/blob/main/examples/esp32/main.cpp)
- [Unit-test guide](https://github.com/PiFou86/UtilifyAnalogInput/blob/main/test/README.md)

Build an example from the command line:

```sh
pio run -e unor3
pio run -e esp32doit-devkit-v1
pio run -e unor4_minima
pio run -e unor4_wifi
```

In VS Code, open the PlatformIO sidebar, expand **PROJECT TASKS**, select an
environment, then choose **General > Build**. Use **Upload and Monitor** to run
the example and inspect its serial output.

## Lifetime and timing notes

- Callback and action code runs synchronously inside the constructor or
  `tick()` and should return quickly.
- Action pointers are not owned; each action must outlive the input using it.
- Joystick calibration blocks for 10 seconds while collecting extrema.
- The default joystick button pin is 0; pass an explicit safe pin for the
  selected board.

## License and author

Released under the MIT license. Created by Pierre-François Léon —
[GitHub](https://github.com/PiFou86) ·
[LinkedIn](https://www.linkedin.com/in/pierrefrancoisleon/)
