# ADC resolution and platform compatibility

[Documentation](README.md) | [Français](../fr/compatibility.md)

## Compile-time resolution

The public Arduino-ESP32 API provides `analogReadResolution(bits)` but no
portable getter for the current returned resolution. UtilifyAnalogInput
therefore uses `UTILIFY_ANALOG_INPUT_BITS`, known at compilation time.

| Platform | Default `UTILIFY_ANALOG_INPUT_BITS` | Normalized consumers |
|---|---:|---|
| Uno R3 AVR | 10 | `Potentiometer`, `DualAxisJoystick` |
| Uno R4 Minima/WiFi | 10 | `Potentiometer`, `DualAxisJoystick` |
| ESP32 | 12 | `Potentiometer`, `DualAxisJoystick` |

`AnalogInput::value()` always remains raw. The define accepts values from 1 to
16; invalid values stop compilation.

For an ESP32 configured to return 11 bits, for example:

```ini
[env:esp32doit-devkit-v1]
platform = espressif32
board = esp32doit-devkit-v1
framework = arduino
build_flags =
    -D UTILIFY_ANALOG_INPUT_BITS=11
```

```cpp
#include <Arduino.h>
#include <Utilify/AnalogInput/Potentiometer.h>

void setup() {
  analogReadResolution(UTILIFY_ANALOG_INPUT_BITS);
}
```

The build flag and `analogReadResolution()` argument must match. The library
does not change this global setting itself.

## Conversion

- More than 10 input bits: shift right by `bits - 10`.
- Exactly 10 bits: preserve the value.
- Fewer than 10 input bits: shift left by `10 - bits`.

The preprocessor selects one expression, so there is no runtime branch or
resolution lookup. A left shift fills low bits with zero, so input resolutions
below 10 do not reach exactly 1023.

## Callback support

| Capability | Uno R3 | Uno R4 | ESP32 |
|---|---|---|---|
| Free function | Yes | Yes | Yes |
| `static` method | Yes | Yes | Yes |
| Non-capturing lambda | Yes | Yes | Yes |
| Capturing lambda | No | Yes | Yes |
| Stateful `ActionBase<T>` | Yes | Yes | Yes |

## Timing

Elapsed-time comparisons use unsigned subtraction, so normal `millis()`
overflow is safe. Calls must not be separated by a complete counter cycle.
