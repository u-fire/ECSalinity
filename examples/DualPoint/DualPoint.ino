/*!
   ufire.co for links to documentation, examples, and libraries
   github.com/u-fire for feature requests, bug reports, and  questions
   questions@ufire.co to get in touch with someone

   This example is compatible with hardware board version 2.

   It show's a user interactive dual point calibration routine.
 */

#include <ECSalinity.h>

EC_Salinity ec;

void calibrateLow() {
  Serial.println("Put the probe in the reference low solution and wait for the readings to stabilize.");
  Serial.println("Enter what the low solution's measurement should be in mS/cm and press enter.");

  while (Serial.available() == 0) {
    ec.measureEC();
    delay(500);
    Serial.print("low mS/cm: "); Serial.println(ec.mS);
  }

  float low_mS = Serial.readStringUntil('/r').toFloat();
  ec.calibrateProbeLow(low_mS, ec.tempCoefEC);
}

void calibrateHigh() {
  Serial.println("Put the probe in the reference high solution and wait for the readings to stabilize.");
  Serial.println("Enter what the high solution's measurement should be in mS/cm and press enter.");

  while (Serial.available() == 0) {
    ec.measureEC();
    delay(500);
    Serial.print("high mS/cm: "); Serial.println(ec.mS);
  }

  float high_mS = Serial.readStringUntil('/r').toFloat();
  ec.calibrateProbeHigh(high_mS, ec.tempCoefEC);
}

void setup() {
  Serial.begin(9600);
  Serial.flush();
  ec.reset();
  calibrateLow();
  calibrateHigh();
  ec.useDualPoint(true);
}

void loop() {
  ec.measureEC();
  Serial.print("mS/cm: "); Serial.println(ec.mS);
  delay(1000);
}
