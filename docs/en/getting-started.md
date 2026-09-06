# Getting started and wiring

[Documentation](README.md) | [Français](../fr/getting-started.md)

## Installation

Add the library to a PlatformIO environment:

```ini
lib_deps =
    sirdrako/UtilifyAnalogInput
```

PlatformIO installs Utilify and UtilifyDigitalInput as dependencies. In Arduino
IDE, install those three libraries with Library Manager.

## Potentiometer wiring

Connect the two outside terminals to the board's supply and `GND`, and the
wiper to an analog pin such as `A0`.

```text
VCC ---- potentiometer ---- GND
               |
               +---------- A0
```

Never apply a voltage above the analog pin's permitted input voltage. ESP32
boards use 3.3 V logic; check the selected board's limits.

## Joystick wiring

Connect `VRx` and `VRy` to two analog-capable pins, `SW` to a safe digital pin,
and the module's supply and ground to the board. The button is active low and
uses `INPUT_PULLUP`. Pass the button pin explicitly; the API's default pin 0
can conflict with boot or serial functions on some boards.

## Cooperative polling

Call `tick()` often from `loop()`. Analog inputs are sampled at most every 5 ms
and callbacks run synchronously. A callback should therefore do little work.

Constructors read the pin immediately and notify a supplied callback or action
once. If objects are global, that notification happens before `setup()` and
before `Serial.begin()`; the complete examples create them during `setup()` for
that reason.

## Callback lifetime

`ActionBase<int>*`, `ActionBase<float>*`, and `ActionBase<void>*` are
non-owning. The library never destroys the user-provided action. It must remain
alive as long as its input or joystick may call it.
