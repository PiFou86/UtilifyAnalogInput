#include <limits.h>
#include <math.h>
#include <unity.h>

#include "fakes/FakeArduino.h"

#include <Utilify/Action/ActionBase.h>
#include <Utilify/AnalogInput/AnalogInput.h>
#include <Utilify/AnalogInput/DualAxisJoystick.h>
#include <Utilify/AnalogInput/Potentiometer.h>

namespace {

constexpr uint8_t ANALOG_PIN_X = A0;
constexpr uint8_t ANALOG_PIN_Y = A1;
constexpr uint8_t BUTTON_PIN = 2;

constexpr int RAW_ANALOG_MAX =
    static_cast<int>((1UL << UTILIFY_ANALOG_INPUT_BITS) - 1UL);
constexpr int RAW_ANALOG_CENTER = 1UL << (UTILIFY_ANALOG_INPUT_BITS - 1);
constexpr int NORMALIZED_ANALOG_MAX =
    NORMALIZE_ANALOG_VALUE_TO_1023(RAW_ANALOG_MAX);

int intCallbackCount;
int lastIntValue;
int xCallbackCount;
int yCallbackCount;
float lastXValue;
float lastYValue;
int buttonPressedCount;
int buttonReleasedCount;
int calibratingCount;
int calibratedCount;

void onIntValueChanged(const int& value) {
  ++intCallbackCount;
  lastIntValue = value;
}

void onXValueChanged(const float& value) {
  ++xCallbackCount;
  lastXValue = value;
}

void onYValueChanged(const float& value) {
  ++yCallbackCount;
  lastYValue = value;
}

void onButtonPressed() { ++buttonPressedCount; }
void onButtonReleased() { ++buttonReleasedCount; }
void onCalibrating() { ++calibratingCount; }
void onCalibrated() { ++calibratedCount; }

template <typename ValueType>
class CounterAction : public ActionBase<ValueType> {
 public:
  CounterAction(int& count, ValueType& lastValue)
      : m_count(count), m_lastValue(lastValue) {}

  void execute(const ValueType& value) override {
    ++m_count;
    m_lastValue = value;
  }

 private:
  int& m_count;
  ValueType& m_lastValue;
};

class VoidCounterAction : public ActionBase<void> {
 public:
  explicit VoidCounterAction(int& count) : m_count(count) {}

  void execute() override { ++m_count; }

 private:
  int& m_count;
};

void advanceReadInterval() { FakeArduino::advanceMillis(5); }

class AnalogInputTests {
 public:
  static void constructeur_ActionFournie_ValeurInitialeNotifiee() {
    FakeArduino::setAnalogValue(ANALOG_PIN_X, 321);
    CounterAction<int> action(intCallbackCount, lastIntValue);

    AnalogInput input(ANALOG_PIN_X, &action);

    TEST_ASSERT_EQUAL_INT(INPUT,
                          FakeArduino::pinModeValue(ANALOG_PIN_X));
    TEST_ASSERT_EQUAL_INT(321, input.value());
    TEST_ASSERT_EQUAL_INT(1, intCallbackCount);
    TEST_ASSERT_EQUAL_INT(321, lastIntValue);
  }

  static void constructeur_CallbackFourni_ValeurInitialeNotifiee() {
    FakeArduino::setAnalogValue(ANALOG_PIN_X, 456);

    AnalogInput input(ANALOG_PIN_X, onIntValueChanged);

    TEST_ASSERT_EQUAL_INT(456, input.value());
    TEST_ASSERT_EQUAL_INT(1, intCallbackCount);
    TEST_ASSERT_EQUAL_INT(456, lastIntValue);
  }

  static void tick_IntervalleLectureNonAtteint_ValeurNonModifiee() {
    FakeArduino::setAnalogValue(ANALOG_PIN_X, 100);
    AnalogInput input(ANALOG_PIN_X, onIntValueChanged);
    intCallbackCount = 0;
    FakeArduino::setAnalogValue(ANALOG_PIN_X, 200);
    FakeArduino::advanceMillis(4);

    input.tick();

    TEST_ASSERT_EQUAL_INT(100, input.value());
    TEST_ASSERT_EQUAL_INT(0, intCallbackCount);
  }

  static void tick_EcartEgalTolerance_ValeurNonModifiee() {
    FakeArduino::setAnalogValue(ANALOG_PIN_X, 100);
    AnalogInput input(ANALOG_PIN_X, onIntValueChanged);
    intCallbackCount = 0;
    input.tolerance(5);
    FakeArduino::setAnalogValue(ANALOG_PIN_X, 105);
    advanceReadInterval();

    input.tick();

    TEST_ASSERT_EQUAL_INT(100, input.value());
    TEST_ASSERT_EQUAL_INT(0, intCallbackCount);
  }

  static void tick_EcartSuperieurTolerance_ValeurEtCallbackModifies() {
    FakeArduino::setAnalogValue(ANALOG_PIN_X, 100);
    AnalogInput input(ANALOG_PIN_X, onIntValueChanged);
    intCallbackCount = 0;
    input.tolerance(5);
    FakeArduino::setAnalogValue(ANALOG_PIN_X, 106);
    advanceReadInterval();

    input.tick();

    TEST_ASSERT_EQUAL_INT(106, input.value());
    TEST_ASSERT_EQUAL_INT(1, intCallbackCount);
    TEST_ASSERT_EQUAL_INT(106, lastIntValue);
  }

  static void tolerance_ValeurNegative_ValeurPrecedenteConservee() {
    AnalogInput input(ANALOG_PIN_X);

    input.tolerance(-1);

    TEST_ASSERT_EQUAL_INT(2, input.tolerance());
  }

  static void setCallbackValueChanged_ActionExistante_CallbackRemplaceAction() {
    int actionCount = 0;
    int lastActionValue = 0;
    CounterAction<int> action(actionCount, lastActionValue);
    AnalogInput input(ANALOG_PIN_X, &action);
    intCallbackCount = 0;
    const int actionCountBefore = actionCount;
    input.setCallbackValueChanged(onIntValueChanged);
    FakeArduino::setAnalogValue(ANALOG_PIN_X, 20);
    advanceReadInterval();

    input.tick();

    TEST_ASSERT_EQUAL_INT(actionCountBefore, actionCount);
    TEST_ASSERT_EQUAL_INT(1, intCallbackCount);
    TEST_ASSERT_EQUAL_INT(20, lastIntValue);
  }

  static void setActionValueChanged_CallbackExistant_ActionRemplaceCallback() {
    int actionCount = 0;
    int lastActionValue = 0;
    CounterAction<int> action(actionCount, lastActionValue);
    AnalogInput input(ANALOG_PIN_X, onIntValueChanged);
    intCallbackCount = 0;
    input.setActionValueChanged(&action);
    FakeArduino::setAnalogValue(ANALOG_PIN_X, 20);
    advanceReadInterval();

    input.tick();

    TEST_ASSERT_EQUAL_INT(1, actionCount);
    TEST_ASSERT_EQUAL_INT(20, lastActionValue);
    TEST_ASSERT_EQUAL_INT(0, intCallbackCount);
  }

  static void tick_MillisDepasseValeurMaximale_LectureEffectuee() {
    FakeArduino::setMillis(ULONG_MAX - 2UL);
    AnalogInput input(ANALOG_PIN_X, onIntValueChanged);
    intCallbackCount = 0;
    FakeArduino::setAnalogValue(ANALOG_PIN_X, 10);
    FakeArduino::advanceMillis(5);

    input.tick();

    TEST_ASSERT_EQUAL_INT(10, input.value());
    TEST_ASSERT_EQUAL_INT(1, intCallbackCount);
  }

  static void normalisation_ResolutionConfiguree_DecalageCompileCorrect() {
    TEST_ASSERT_EQUAL_INT(
        NORMALIZED_ANALOG_MAX,
        NORMALIZE_ANALOG_VALUE_TO_1023(RAW_ANALOG_MAX));
    TEST_ASSERT_EQUAL_INT(
        512, NORMALIZE_ANALOG_VALUE_TO_1023(RAW_ANALOG_CENTER));
  }
};

class PotentiometerTests {
 public:
  static void value_ValeurAnalogiqueMaximale_RetourneMaximumNormalise() {
    FakeArduino::setAnalogValue(ANALOG_PIN_X, RAW_ANALOG_MAX);

    Potentiometer potentiometer(ANALOG_PIN_X);

    TEST_ASSERT_EQUAL_INT(NORMALIZED_ANALOG_MAX, potentiometer.value());
  }

  static void constructeur_CallbackFourni_NotifieValeurInitialeNormalisee() {
    FakeArduino::setAnalogValue(ANALOG_PIN_X, RAW_ANALOG_MAX);

    Potentiometer potentiometer(ANALOG_PIN_X, onIntValueChanged);

    TEST_ASSERT_EQUAL_INT(1, intCallbackCount);
    TEST_ASSERT_EQUAL_INT(NORMALIZED_ANALOG_MAX, lastIntValue);
  }

  static void tick_NouvelleValeur_NotifieValeurNormalisee() {
    FakeArduino::setAnalogValue(ANALOG_PIN_X, RAW_ANALOG_CENTER);
    Potentiometer potentiometer(ANALOG_PIN_X, onIntValueChanged);
    intCallbackCount = 0;
    FakeArduino::setAnalogValue(ANALOG_PIN_X, RAW_ANALOG_MAX);
    advanceReadInterval();

    potentiometer.tick();

    TEST_ASSERT_EQUAL_INT(1, intCallbackCount);
    TEST_ASSERT_EQUAL_INT(NORMALIZED_ANALOG_MAX, lastIntValue);
  }
};

class DualAxisJoystickTests {
 public:
  static void valueX_ValeursMinimaleCentraleMaximale_RetourneMoinsUnZeroUn() {
    FakeArduino::setAnalogValue(ANALOG_PIN_X, RAW_ANALOG_CENTER);
    FakeArduino::setAnalogValue(ANALOG_PIN_Y, RAW_ANALOG_CENTER);
    DualAxisJoystick joystick(ANALOG_PIN_X, ANALOG_PIN_Y, BUTTON_PIN,
                              onXValueChanged, onYValueChanged,
                              onButtonPressed, onButtonReleased);

    TEST_ASSERT_FLOAT_WITHIN(0.01F, 0.0F, joystick.valueX());

    FakeArduino::setAnalogValue(ANALOG_PIN_X, RAW_ANALOG_MAX);
    advanceReadInterval();
    joystick.tick();
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 1.0F, joystick.valueX());

    FakeArduino::setAnalogValue(ANALOG_PIN_X, 0);
    advanceReadInterval();
    joystick.tick();
    TEST_ASSERT_FLOAT_WITHIN(0.01F, -1.0F, joystick.valueX());
  }

  static void invertX_AxeInverse_SigneValeurInverse() {
    FakeArduino::setAnalogValue(ANALOG_PIN_X, RAW_ANALOG_MAX);
    FakeArduino::setAnalogValue(ANALOG_PIN_Y, RAW_ANALOG_CENTER);
    DualAxisJoystick joystick(ANALOG_PIN_X, ANALOG_PIN_Y, BUTTON_PIN,
                              onXValueChanged, onYValueChanged,
                              onButtonPressed, onButtonReleased);

    joystick.invertX(true);

    TEST_ASSERT_FLOAT_WITHIN(0.01F, -1.0F, joystick.valueX());
  }

  static void valueY_ValeursMinimaleCentraleMaximale_RetourneMoinsUnZeroUn() {
    FakeArduino::setAnalogValue(ANALOG_PIN_X, RAW_ANALOG_CENTER);
    FakeArduino::setAnalogValue(ANALOG_PIN_Y, RAW_ANALOG_CENTER);
    DualAxisJoystick joystick(ANALOG_PIN_X, ANALOG_PIN_Y, BUTTON_PIN,
                              onXValueChanged, onYValueChanged,
                              onButtonPressed, onButtonReleased);

    TEST_ASSERT_FLOAT_WITHIN(0.01F, 0.0F, joystick.valueY());

    FakeArduino::setAnalogValue(ANALOG_PIN_Y, RAW_ANALOG_MAX);
    advanceReadInterval();
    joystick.tick();
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 1.0F, joystick.valueY());

    FakeArduino::setAnalogValue(ANALOG_PIN_Y, 0);
    advanceReadInterval();
    joystick.tick();
    TEST_ASSERT_FLOAT_WITHIN(0.01F, -1.0F, joystick.valueY());
  }

  static void invertY_AxeInverse_SigneValeurInverse() {
    FakeArduino::setAnalogValue(ANALOG_PIN_X, RAW_ANALOG_CENTER);
    FakeArduino::setAnalogValue(ANALOG_PIN_Y, RAW_ANALOG_MAX);
    DualAxisJoystick joystick(ANALOG_PIN_X, ANALOG_PIN_Y, BUTTON_PIN,
                              onXValueChanged, onYValueChanged,
                              onButtonPressed, onButtonReleased);

    joystick.invertY(true);

    TEST_ASSERT_FLOAT_WITHIN(0.01F, -1.0F, joystick.valueY());
  }

  static void tick_AxeXModifie_CallbackValeurNormaliseeInvoque() {
    FakeArduino::setAnalogValue(ANALOG_PIN_X, RAW_ANALOG_CENTER);
    FakeArduino::setAnalogValue(ANALOG_PIN_Y, RAW_ANALOG_CENTER);
    DualAxisJoystick joystick(ANALOG_PIN_X, ANALOG_PIN_Y, BUTTON_PIN,
                              onXValueChanged, onYValueChanged,
                              onButtonPressed, onButtonReleased);
    FakeArduino::setAnalogValue(ANALOG_PIN_X, RAW_ANALOG_MAX);
    advanceReadInterval();

    joystick.tick();

    TEST_ASSERT_EQUAL_INT(1, xCallbackCount);
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 1.0F, lastXValue);
    TEST_ASSERT_EQUAL_INT(0, yCallbackCount);
  }

  static void tick_BoutonEnfonce_CallbackPressedInvoque() {
    DualAxisJoystick joystick(ANALOG_PIN_X, ANALOG_PIN_Y, BUTTON_PIN,
                              onXValueChanged, onYValueChanged,
                              onButtonPressed, onButtonReleased);
    FakeArduino::setDigitalValue(BUTTON_PIN, LOW);

    joystick.tick();
    FakeArduino::advanceMillis(26);
    joystick.tick();

    TEST_ASSERT_EQUAL_INT(1, buttonPressedCount);
  }

  static void tick_BoutonRelache_CallbackReleasedInvoque() {
    DualAxisJoystick joystick(ANALOG_PIN_X, ANALOG_PIN_Y, BUTTON_PIN,
                              onXValueChanged, onYValueChanged,
                              onButtonPressed, onButtonReleased);
    FakeArduino::setDigitalValue(BUTTON_PIN, LOW);
    joystick.tick();
    FakeArduino::setDigitalValue(BUTTON_PIN, HIGH);

    joystick.tick();

    TEST_ASSERT_EQUAL_INT(1, buttonReleasedCount);
  }

  static void constructeur_ActionsFournies_ActionAxeXInvoquee() {
    CounterAction<float> xAction(xCallbackCount, lastXValue);
    CounterAction<float> yAction(yCallbackCount, lastYValue);
    VoidCounterAction pressedAction(buttonPressedCount);
    VoidCounterAction releasedAction(buttonReleasedCount);
    DualAxisJoystick joystick(ANALOG_PIN_X, ANALOG_PIN_Y, BUTTON_PIN,
                              &xAction, &yAction, &pressedAction,
                              &releasedAction);
    FakeArduino::setAnalogValue(ANALOG_PIN_X, RAW_ANALOG_MAX);
    advanceReadInterval();

    joystick.tick();

    TEST_ASSERT_EQUAL_INT(1, xCallbackCount);
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 1.0F, lastXValue);
  }

  static void calibrate_CallbacksFournis_CallbacksInvoques() {
    FakeArduino::setAnalogValue(ANALOG_PIN_X, RAW_ANALOG_CENTER);
    FakeArduino::setAnalogValue(ANALOG_PIN_Y, RAW_ANALOG_CENTER);
    DualAxisJoystick joystick(ANALOG_PIN_X, ANALOG_PIN_Y, BUTTON_PIN,
                              onXValueChanged, onYValueChanged,
                              onButtonPressed, onButtonReleased);
    FakeArduino::setMillisAutoAdvance(1);

    joystick.calibrate(onCalibrating, onCalibrated);

    TEST_ASSERT_EQUAL_INT(1, calibratingCount);
    TEST_ASSERT_EQUAL_INT(1, calibratedCount);
  }

  static void calibrate_ActionsFournies_ActionsInvoquees() {
    DualAxisJoystick joystick(ANALOG_PIN_X, ANALOG_PIN_Y);
    VoidCounterAction calibratingAction(calibratingCount);
    VoidCounterAction calibratedAction(calibratedCount);
    FakeArduino::setMillisAutoAdvance(1);

    joystick.calibrate(&calibratingAction, &calibratedAction);

    TEST_ASSERT_EQUAL_INT(1, calibratingCount);
    TEST_ASSERT_EQUAL_INT(1, calibratedCount);
  }

  static void calibrate_AxesImmobiles_ValeursCentralesFiniesEtNulles() {
    FakeArduino::setAnalogValue(ANALOG_PIN_X, RAW_ANALOG_CENTER);
    FakeArduino::setAnalogValue(ANALOG_PIN_Y, RAW_ANALOG_CENTER);
    DualAxisJoystick joystick(ANALOG_PIN_X, ANALOG_PIN_Y);
    FakeArduino::setMillisAutoAdvance(1);

    joystick.calibrate();

    TEST_ASSERT_TRUE(isfinite(joystick.valueX()));
    TEST_ASSERT_TRUE(isfinite(joystick.valueY()));
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 0.0F, joystick.valueX());
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 0.0F, joystick.valueY());
  }

  static void calibrate_PlagesCompletes_ValeurCentraleRetourneZero() {
    FakeArduino::setAnalogValue(ANALOG_PIN_X, RAW_ANALOG_CENTER);
    FakeArduino::setAnalogValue(ANALOG_PIN_Y, RAW_ANALOG_CENTER);
    DualAxisJoystick joystick(ANALOG_PIN_X, ANALOG_PIN_Y);
    FakeArduino::setAlternatingAnalogValues(ANALOG_PIN_X, 0, RAW_ANALOG_MAX);
    FakeArduino::setAlternatingAnalogValues(ANALOG_PIN_Y, 0, RAW_ANALOG_MAX);
    FakeArduino::setMillisAutoAdvance(1);
    joystick.calibrate();
    FakeArduino::setMillisAutoAdvance(0);
    FakeArduino::clearAlternatingAnalogValues(ANALOG_PIN_X);
    FakeArduino::clearAlternatingAnalogValues(ANALOG_PIN_Y);
    FakeArduino::setAnalogValue(ANALOG_PIN_X, RAW_ANALOG_CENTER);
    FakeArduino::setAnalogValue(ANALOG_PIN_Y, RAW_ANALOG_CENTER);
    advanceReadInterval();

    joystick.tick();

    TEST_ASSERT_FLOAT_WITHIN(0.01F, 0.0F, joystick.valueX());
    TEST_ASSERT_FLOAT_WITHIN(0.01F, 0.0F, joystick.valueY());
  }

  static void calibrate_PlageYAsymetrique_ValeurMaximaleRetourneUn() {
    constexpr int normalizedMin = 100;
    constexpr int normalizedMax = 900;
#if UTILIFY_ANALOG_INPUT_BITS >= 10
    constexpr int rawMin =
        normalizedMin << (UTILIFY_ANALOG_INPUT_BITS - 10);
    constexpr int rawMax =
        normalizedMax << (UTILIFY_ANALOG_INPUT_BITS - 10);
#else
    constexpr int rawMin =
        normalizedMin >> (10 - UTILIFY_ANALOG_INPUT_BITS);
    constexpr int rawMax =
        normalizedMax >> (10 - UTILIFY_ANALOG_INPUT_BITS);
#endif
    FakeArduino::setAnalogValue(ANALOG_PIN_X, RAW_ANALOG_CENTER);
    FakeArduino::setAnalogValue(ANALOG_PIN_Y, RAW_ANALOG_CENTER);
    DualAxisJoystick joystick(ANALOG_PIN_X, ANALOG_PIN_Y);
    FakeArduino::setAlternatingAnalogValues(ANALOG_PIN_X, 0, RAW_ANALOG_MAX);
    FakeArduino::setAlternatingAnalogValues(ANALOG_PIN_Y, rawMin, rawMax);
    FakeArduino::setMillisAutoAdvance(1);
    joystick.calibrate();
    FakeArduino::setMillisAutoAdvance(0);
    FakeArduino::clearAlternatingAnalogValues(ANALOG_PIN_X);
    FakeArduino::clearAlternatingAnalogValues(ANALOG_PIN_Y);
    FakeArduino::setAnalogValue(ANALOG_PIN_Y, rawMax);
    advanceReadInterval();

    joystick.tick();

    TEST_ASSERT_FLOAT_WITHIN(0.01F, 1.0F, joystick.valueY());
  }
};

void resetTestState() {
  FakeArduino::reset();
  intCallbackCount = 0;
  lastIntValue = 0;
  xCallbackCount = 0;
  yCallbackCount = 0;
  lastXValue = 0.0F;
  lastYValue = 0.0F;
  buttonPressedCount = 0;
  buttonReleasedCount = 0;
  calibratingCount = 0;
  calibratedCount = 0;
}

}  // namespace

void setUp() { resetTestState(); }

void tearDown() {}

int main(int, char**) {
  UNITY_BEGIN();
  RUN_TEST(
      AnalogInputTests::constructeur_ActionFournie_ValeurInitialeNotifiee);
  RUN_TEST(
      AnalogInputTests::constructeur_CallbackFourni_ValeurInitialeNotifiee);
  RUN_TEST(
      AnalogInputTests::tick_IntervalleLectureNonAtteint_ValeurNonModifiee);
  RUN_TEST(AnalogInputTests::tick_EcartEgalTolerance_ValeurNonModifiee);
  RUN_TEST(
      AnalogInputTests::tick_EcartSuperieurTolerance_ValeurEtCallbackModifies);
  RUN_TEST(
      AnalogInputTests::tolerance_ValeurNegative_ValeurPrecedenteConservee);
  RUN_TEST(
      AnalogInputTests::setCallbackValueChanged_ActionExistante_CallbackRemplaceAction);
  RUN_TEST(
      AnalogInputTests::setActionValueChanged_CallbackExistant_ActionRemplaceCallback);
  RUN_TEST(AnalogInputTests::tick_MillisDepasseValeurMaximale_LectureEffectuee);
  RUN_TEST(
      AnalogInputTests::normalisation_ResolutionConfiguree_DecalageCompileCorrect);
  RUN_TEST(
      PotentiometerTests::value_ValeurAnalogiqueMaximale_RetourneMaximumNormalise);
  RUN_TEST(
      PotentiometerTests::constructeur_CallbackFourni_NotifieValeurInitialeNormalisee);
  RUN_TEST(PotentiometerTests::tick_NouvelleValeur_NotifieValeurNormalisee);
  RUN_TEST(
      DualAxisJoystickTests::valueX_ValeursMinimaleCentraleMaximale_RetourneMoinsUnZeroUn);
  RUN_TEST(DualAxisJoystickTests::invertX_AxeInverse_SigneValeurInverse);
  RUN_TEST(
      DualAxisJoystickTests::valueY_ValeursMinimaleCentraleMaximale_RetourneMoinsUnZeroUn);
  RUN_TEST(DualAxisJoystickTests::invertY_AxeInverse_SigneValeurInverse);
  RUN_TEST(
      DualAxisJoystickTests::tick_AxeXModifie_CallbackValeurNormaliseeInvoque);
  RUN_TEST(DualAxisJoystickTests::tick_BoutonEnfonce_CallbackPressedInvoque);
  RUN_TEST(DualAxisJoystickTests::tick_BoutonRelache_CallbackReleasedInvoque);
  RUN_TEST(
      DualAxisJoystickTests::constructeur_ActionsFournies_ActionAxeXInvoquee);
  RUN_TEST(
      DualAxisJoystickTests::calibrate_CallbacksFournis_CallbacksInvoques);
  RUN_TEST(DualAxisJoystickTests::calibrate_ActionsFournies_ActionsInvoquees);
  RUN_TEST(
      DualAxisJoystickTests::calibrate_AxesImmobiles_ValeursCentralesFiniesEtNulles);
  RUN_TEST(
      DualAxisJoystickTests::calibrate_PlagesCompletes_ValeurCentraleRetourneZero);
  RUN_TEST(
      DualAxisJoystickTests::calibrate_PlageYAsymetrique_ValeurMaximaleRetourneUn);
  return UNITY_END();
}
