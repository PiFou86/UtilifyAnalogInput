# Résolution du CAN et compatibilité

[Documentation](README.md) | [English](../en/compatibility.md)

## Résolution connue à la compilation

L’API publique Arduino-ESP32 fournit `analogReadResolution(bits)`, mais aucun
accesseur portable permettant de connaître la résolution actuellement
retournée. UtilifyAnalogInput emploie donc `UTILIFY_ANALOG_INPUT_BITS`, connu à
la compilation.

| Plateforme | `UTILIFY_ANALOG_INPUT_BITS` par défaut | Types normalisés |
|---|---:|---|
| Uno R3 AVR | 10 | `Potentiometer`, `DualAxisJoystick` |
| Uno R4 Minima/WiFi | 10 | `Potentiometer`, `DualAxisJoystick` |
| ESP32 | 12 | `Potentiometer`, `DualAxisJoystick` |

`AnalogInput::value()` reste toujours brut. Le define accepte les valeurs de 1
à 16 ; toute valeur incorrecte arrête la compilation.

Pour un ESP32 configuré afin de renvoyer 11 bits, par exemple :

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

Le build flag et l’argument d’`analogReadResolution()` doivent correspondre. La
bibliothèque ne modifie pas elle-même cette configuration globale.

## Conversion

- Plus de 10 bits en entrée : décalage à droite de `bits - 10`.
- Exactement 10 bits : valeur inchangée.
- Moins de 10 bits : décalage à gauche de `10 - bits`.

Le préprocesseur choisit une seule expression : aucune branche ni recherche de
résolution n’est exécutée. Un décalage à gauche remplit les bits faibles avec
des zéros ; une résolution inférieure à 10 bits n’atteint donc pas exactement
1023.

## Prise en charge des callbacks

| Possibilité | Uno R3 | Uno R4 | ESP32 |
|---|---|---|---|
| Fonction libre | Oui | Oui | Oui |
| Méthode `static` | Oui | Oui | Oui |
| Lambda sans capture | Oui | Oui | Oui |
| Lambda capturante | Non | Oui | Oui |
| `ActionBase<T>` avec état | Oui | Oui | Oui |

## Temporisation

Les comparaisons de temps utilisent une soustraction non signée : le
débordement normal de `millis()` est sûr. Deux appels ne doivent pas être
séparés par un cycle complet du compteur.
