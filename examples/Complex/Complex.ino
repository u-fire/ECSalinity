/*!
   ufire.co for links to documentation, examples, and libraries
   github.com/u-fire for feature requests, bug reports, and  questions
   questions@ufire.co to get in touch with someone

   This example is compatible with hardware board version 1c
 */

#include <ECSalinity.h>

EC_Salinity ec;
float calibrationSolution_mS = 2.77;

void setup()
{
  Serial.begin(9600);
  Serial.println("Calibrating...");
  ec.setK(1.0);
  ec.calibrateProbe(calibrationSolution_mS, ec.tempCoefEC);
  Serial.print("  offset: "); Serial.println(ec.getCalibrateOffset());
}

void loop()
{
  ec.measureEC();
  Serial.print("mS: "); Serial.println(ec.mS);
  Serial.print("TDS: "); Serial.println(ec.PPM_500);
  Serial.print("Temp C/F: "); Serial.print(ec.tempC); Serial.print("/");
  Serial.println(ec.tempF);
  Serial.println("-----");
  delay(10000);
}
