# Tests unitaires de UtilifyAnalogInput

Les tests Unity couvrent les trois classes publiques :

- `AnalogInputTests` pour `AnalogInput` ;
- `PotentiometerTests` pour `Potentiometer` ;
- `DualAxisJoystickTests` pour `DualAxisJoystick`.

Chaque méthode suit la convention :

```text
MethodeTestee_DescriptionCas_ResultatAttendu
```

## Simulation native

Une carte seule ne peut pas produire plusieurs tensions analogiques stables sur
ses propres entrées. Les tests remplacent donc les fonctions Arduino utilisées
par la bibliothèque :

- `analogRead()` fournit une valeur ou une séquence contrôlée ;
- `millis()` permet de tester l’intervalle, son débordement et les dix secondes
  logiques de calibration sans attendre ;
- `digitalRead()` simule le bouton du joystick ;
- `pinMode()` enregistre la configuration des broches.

Aucune carte, aucun potentiomètre, aucun joystick et aucun fil ne sont requis.
Le faux `PushButton` vérifie l’intégration du bouton ; l’anti-rebond propre à
UtilifyDigitalInput est testé dans le dépôt de cette bibliothèque.

## Lancer les tests

La matrice couvre Uno R3 ainsi que les conversions ESP32 en 9, 10, 12 et
16 bits :

```sh
pio test -e native-unor3-tests
pio test -e native-esp32-9bit-tests
pio test -e native-esp32-10bit-tests
pio test -e native-esp32-tests
pio test -e native-esp32-16bit-tests
```

Pour tout lancer en une commande :

```sh
pio test \
  -e native-unor3-tests \
  -e native-esp32-9bit-tests \
  -e native-esp32-10bit-tests \
  -e native-esp32-tests \
  -e native-esp32-16bit-tests
```

Dans VS Code, ouvrez la barre latérale PlatformIO, développez **PROJECT TASKS**,
choisissez le profil voulu, puis **Advanced > Test**.

## Comportements couverts

- lecture et notification initiales par callback ou `ActionBase` ;
- remplacement mutuel d’un callback et d’une action ;
- intervalle de lecture de 5 ms et débordement de `millis()` ;
- changements inférieurs, égaux ou supérieurs à la tolérance ;
- rejet d’une tolérance négative ;
- normalisation du potentiomètre, y compris les notifications ;
- conversion à la compilation vers 10 bits dans les deux directions ;
- valeurs minimale, centrale et maximale des deux axes ;
- inversion de chaque axe ;
- appui et relâchement du bouton ;
- calibration par callbacks ou actions ;
- calibration immobile, complète et asymétrique.

Chaque profil exécute 26 cas. La matrice complète représente 130 exécutions.
