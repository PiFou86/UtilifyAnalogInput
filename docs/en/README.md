# UtilifyAnalogInput documentation

[English](README.md) | [Français](../fr/README.md) | [Project home](../../README.md)

UtilifyAnalogInput reads raw analog pins, normalizes potentiometers, and handles
calibrated two-axis joysticks on Uno R3, Uno R4, and ESP32.

## Guides

- [Getting started and wiring](getting-started.md)
- [`AnalogInput` and `Potentiometer`](analog-inputs.md)
- [`DualAxisJoystick`](dual-axis-joystick.md)
- [ADC resolution and platform compatibility](compatibility.md)

## Complete examples

- [Arduino Uno R3](../../examples/unor3/main.cpp): free functions and a portable
  `ActionBase<int>` object.
- [ESP32](../../examples/esp32/main.cpp): capturing lambdas and an explicitly
  configured ADC resolution.

## Build from VS Code

Open the PlatformIO sidebar, expand **PROJECT TASKS**, select `unor3`,
`esp32doit-devkit-v1`, `unor4_minima`, or `unor4_wifi`, then choose
**General > Build**. Use **Upload and Monitor** to run an example.

The simulated resolution matrix and test commands are described in the
[unit-test guide](../../test/README.md).
