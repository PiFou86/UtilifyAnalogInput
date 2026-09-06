# `DualAxisJoystick`

[Documentation](README.md) | [Français](../fr/dual-axis-joystick.md)

## Header and construction

```cpp
#include <Utilify/AnalogInput/DualAxisJoystick.h>
```

The constructor receives X, Y, and button pins, followed by X/Y change handlers
and button press/release handlers. Axis handlers receive a `float`.

```cpp
DualAxisJoystick joystick(A0, A1, 2,
    xChanged, yChanged, buttonPressed, buttonReleased);

void loop() {
  joystick.tick();
}
```

Before calibration, `valueX()` and `valueY()` use a center of 512 and a
10-bit range. Values are approximately `-1.0` at the low end, `0.0` at center,
and `1.0` at the high end. Use `invertX(true)` or `invertY(true)` to reverse an
axis.

## Calibration

```cpp
joystick.calibrate(calibrationStarted, calibrationFinished);
```

Calibration records the current position as center, then samples both axes for
10 seconds. Move the stick through its complete range during that period. The
call is blocking: normal work in `loop()` resumes only after calibration.
Axis-change notifications are suppressed while calibration is active.

If an axis does not move, its value remains finite and returns zero at its
center; no division by zero occurs. Calibration does not persist across a
restart.

Both callback and `ActionBase<void>*` overloads are available for the start and
completion notifications. Null handlers are accepted.

## Button

The joystick delegates its active-low button to UtilifyDigitalInput's
`PushButton`. Connect the switch between the chosen pin and `GND`. Button events
follow that library's debounce behavior.

## Platform callbacks

Uno R3 callbacks are function pointers: use free functions, `static` methods,
or non-capturing lambdas. Uno R4 and ESP32 use `std::function`, so a capturing
lambda can call an instance method. `ActionBase<float>` and `ActionBase<void>`
provide the portable stateful alternative.
