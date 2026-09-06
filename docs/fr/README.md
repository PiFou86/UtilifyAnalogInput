# Documentation de UtilifyAnalogInput

[English](../en/README.md) | [Français](README.md) | [Accueil](../../README.md)

UtilifyAnalogInput lit des entrées analogiques brutes, normalise des
potentiomètres et gère des joysticks calibrés à deux axes sur Uno R3, Uno R4 et
ESP32.

## Guides

- [Démarrage et branchements](getting-started.md)
- [`AnalogInput` et `Potentiometer`](analog-inputs.md)
- [`DualAxisJoystick`](dual-axis-joystick.md)
- [Résolution du CAN et compatibilité](compatibility.md)

## Exemples complets

- [Arduino Uno R3](../../examples/unor3/main.cpp) : fonctions libres et objet
  portable `ActionBase<int>`.
- [ESP32](../../examples/esp32/main.cpp) : lambdas capturantes et résolution du
  CAN configurée explicitement.

## Compiler depuis VS Code

Ouvrez la barre latérale PlatformIO, développez **PROJECT TASKS**, choisissez
`unor3`, `esp32doit-devkit-v1`, `unor4_minima` ou `unor4_wifi`, puis
**General > Build**. Utilisez **Upload and Monitor** pour exécuter un exemple.

La matrice simulée des résolutions et les commandes sont présentées dans le
[guide des tests unitaires](../../test/README.md).
