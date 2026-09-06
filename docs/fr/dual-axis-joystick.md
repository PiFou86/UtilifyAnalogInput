# `DualAxisJoystick`

[Documentation](README.md) | [English](../en/dual-axis-joystick.md)

## En-tête et construction

```cpp
#include <Utilify/AnalogInput/DualAxisJoystick.h>
```

Le constructeur reçoit les broches X, Y et du bouton, puis les gestionnaires
des changements X/Y et ceux de l’appui et du relâchement. Les gestionnaires
d’axe reçoivent un `float`.

```cpp
DualAxisJoystick joystick(A0, A1, 2,
    xChanged, yChanged, buttonPressed, buttonReleased);

void loop() {
  joystick.tick();
}
```

Avant calibration, `valueX()` et `valueY()` emploient un centre de 512 et un
domaine sur 10 bits. Les valeurs sont approximativement `-1.0` au minimum,
`0.0` au centre et `1.0` au maximum. `invertX(true)` et `invertY(true)`
permettent d’inverser les axes.

## Calibration

```cpp
joystick.calibrate(calibrationStarted, calibrationFinished);
```

La calibration enregistre la position actuelle comme centre, puis lit les deux
axes pendant 10 secondes. Déplacez le levier sur toute sa course pendant cette
période. L’appel est bloquant : le travail normal de `loop()` ne reprend
qu’après la calibration. Les notifications des axes sont suspendues pendant
celle-ci.

Si un axe ne bouge pas, sa valeur reste finie et vaut zéro au centre ; aucune
division par zéro ne se produit. La calibration n’est pas conservée après un
redémarrage.

Des surcharges avec callbacks et avec `ActionBase<void>*` sont disponibles pour
les notifications de début et de fin. Les gestionnaires nuls sont acceptés.

## Bouton

Le joystick délègue son bouton actif à l’état bas au `PushButton` de
UtilifyDigitalInput. Branchez le contact entre la broche choisie et `GND`. Les
événements suivent le comportement d’anti-rebond de cette bibliothèque.

## Callbacks selon la plateforme

Sur Uno R3, les callbacks sont des pointeurs de fonction : utilisez une
fonction libre, une méthode `static` ou une lambda sans capture. Uno R4 et ESP32
emploient `std::function` et acceptent une lambda capturant un objet.
`ActionBase<float>` et `ActionBase<void>` sont l’alternative portable avec état.
