# `AnalogInput` and `Potentiometer`

[Documentation](README.md) | [Français](../fr/analog-inputs.md)

## Headers

```cpp
#include <Utilify/AnalogInput/AnalogInput.h>
#include <Utilify/AnalogInput/Potentiometer.h>
```

## `AnalogInput`

`AnalogInput` keeps the raw value returned by `analogRead()`. It checks the pin
every 5 ms when `tick()` is called and accepts a new value only when the
absolute difference is greater than `tolerance()`.

```cpp
void rawChanged(const int& value) {
  Serial.println(value);
}

AnalogInput input(A0, rawChanged);

void setup() {
  input.tolerance(4);
}

void loop() {
  input.tick();
}
```

The default tolerance is 2. A difference exactly equal to the tolerance is
ignored. Negative values are rejected and leave the previous tolerance
unchanged. `value()` returns the latest accepted raw value.

`setCallbackValueChanged()` and `setActionValueChanged()` replace each other:
only the most recently configured notification mechanism is active.

## `Potentiometer`

`Potentiometer` inherits the polling and tolerance behavior of `AnalogInput`,
but `value()` and notifications use a normalized 10-bit domain.

```cpp
Potentiometer potentiometer(A0, rawChanged);

void loop() {
  potentiometer.tick();
}
```

With 10 bits or more at the ADC input, the exposed range is `0–1023`. With a
source below 10 bits, conversion uses a left shift: an 8-bit source reaches
1020 and a 9-bit source reaches 1022. This preserves a very small and fast
conversion on constrained boards.

The tolerance is evaluated on raw readings. On a 12-bit ESP32, tolerance 4
therefore describes four raw ADC counts, while the reported potentiometer value
is shifted to 10 bits.

## Actions

Use `ActionBase<int>` when a portable callback must keep state or invoke an
object, especially on Uno R3 where capturing lambdas are unavailable:

```cpp
class ValuePrinter : public ActionBase<int> {
 public:
  void execute(const int& value) override {
    Serial.println(value);
  }
};

ValuePrinter printer;
Potentiometer potentiometer(A0, &printer);
```

The action is not owned and must outlive the potentiometer.
