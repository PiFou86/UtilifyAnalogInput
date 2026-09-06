# Démarrage et branchements

[Documentation](README.md) | [English](../en/getting-started.md)

## Installation

Ajoutez la bibliothèque à un environnement PlatformIO :

```ini
lib_deps =
    sirdrako/UtilifyAnalogInput
```

PlatformIO installe Utilify et UtilifyDigitalInput comme dépendances. Dans
l’IDE Arduino, installez ces trois bibliothèques avec le gestionnaire de
bibliothèques.

## Branchement d’un potentiomètre

Reliez les deux bornes extérieures à l’alimentation de la carte et à `GND`, et
le curseur à une broche analogique telle que `A0`.

```text
VCC ---- potentiomètre ---- GND
               |
               +------------ A0
```

N’appliquez jamais une tension supérieure à celle admise par l’entrée. Les
cartes ESP32 fonctionnent en logique 3,3 V ; vérifiez les limites de la carte.

## Branchement d’un joystick

Reliez `VRx` et `VRy` à deux broches analogiques, `SW` à une broche numérique
sûre, puis l’alimentation et la masse du module à la carte. Le bouton est actif
à l’état bas et utilise `INPUT_PULLUP`. Précisez sa broche : la valeur 0 par
défaut de l’API peut entrer en conflit avec le démarrage ou le port série.

## Interrogation coopérative

Appelez souvent `tick()` depuis `loop()`. Les entrées analogiques sont lues au
plus toutes les 5 ms et les callbacks s’exécutent de manière synchrone. Ils
doivent donc rester courts.

Les constructeurs lisent immédiatement la broche et notifient une fois le
callback ou l’action fourni. Pour un objet global, cette notification précède
`setup()` et `Serial.begin()` ; les exemples complets créent donc leurs objets
dans `setup()`.

## Durée de vie des callbacks

Les pointeurs `ActionBase<int>*`, `ActionBase<float>*` et `ActionBase<void>*`
ne sont pas possédés par la bibliothèque. L’action doit rester valide aussi
longtemps que l’entrée ou le joystick peut l’appeler.
