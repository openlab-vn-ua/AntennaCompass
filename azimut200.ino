#include <SPHMC8337L2.h>

#include <Wire.h>

#include "BlinkingLED.h"
#include "CompassProcess.h"
#include "BearingDetector.h"
#include "BuzzerProcess.h"
#include "Button.h"
#include "ButtonProcess.h"

#define LED_A_PIN   (6)
#define BUZZ_PIN    (5)
#define BUTTON_PIN  (2)
#define COMPASS_POWER_PIN (10)

const int Compass_i2cAddr = 0x1E;

Energy EnergyA;
static BlinkingLED LedA(LED_A_PIN);
static CompassProcess CompassA(Compass_i2cAddr, COMPASS_POWER_PIN);
static BuzzerProcess BuzzA(BUZZ_PIN);
static BearingDetector DetectA(&LedA, &BuzzA, &CompassA);
static Button ButtonA(BUTTON_PIN);
static ButtonProcess ProcessA(&ButtonA, &EnergyA, &LowPower, &CompassA, &LedA);


void setup()
 {
  Serial.begin(9600);
  Wire.begin();
  LedA.setup();
  CompassA.setup();  
  BuzzA.setup();
  DetectA.setup();
  ButtonA.setup();
  ProcessA.setup();
 }
 
void loop ()
{ 
 CompassA.loopStep();
 LedA.loopStep();
 BuzzA.loopStep();
 ButtonA.loopStep();
 DetectA.loopStep();
 ProcessA.loopStep();
}
