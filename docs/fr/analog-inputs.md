# `AnalogInput` et `Potentiometer`

[Documentation](README.md) | [English](../en/analog-inputs.md)

## En-têtes

```cpp
#include <Utilify/AnalogInput/AnalogInput.h>
#include <Utilify/AnalogInput/Potentiometer.h>
```

## `AnalogInput`

`AnalogInput` conserve la valeur brute renvoyée par `analogRead()`. Il vérifie
la broche toutes les 5 ms lorsque `tick()` est appelé et n’accepte une nouvelle
valeur que si la différence absolue est supérieure à `tolerance()`.

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

La tolérance par défaut vaut 2. Une différence exactement égale à cette valeur
est ignorée. Une valeur négative est refusée et laisse la tolérance précédente
inchangée. `value()` renvoie la dernière valeur brute acceptée.

`setCallbackValueChanged()` et `setActionValueChanged()` se remplacent
mutuellement : seul le dernier mécanisme configuré demeure actif.

## `Potentiometer`

`Potentiometer` hérite de l’interrogation et de la tolérance d’`AnalogInput`,
mais `value()` et ses notifications utilisent un domaine normalisé sur 10 bits.

```cpp
Potentiometer potentiometer(A0, rawChanged);

void loop() {
  potentiometer.tick();
}
```

Avec au moins 10 bits en entrée du CAN, le domaine exposé est `0–1023`. Pour
une source inférieure à 10 bits, la conversion utilise un décalage à gauche :
une source 8 bits atteint 1020 et une source 9 bits atteint 1022. La conversion
reste ainsi très petite et rapide sur les cartes contraintes.

La tolérance est évaluée sur les lectures brutes. Sur un ESP32 à 12 bits, une
tolérance de 4 représente donc quatre pas bruts du CAN, même si la valeur du
potentiomètre est ensuite ramenée sur 10 bits.

## Actions

Utilisez `ActionBase<int>` lorsqu’un callback portable doit conserver un état
ou appeler un objet, particulièrement sur Uno R3 où les lambdas capturantes ne
sont pas disponibles :

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

L’action n’est pas possédée et doit vivre plus longtemps que le potentiomètre.
