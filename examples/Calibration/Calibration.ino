/*!
   ufire.co for links to documentation, examples, and libraries
   github.com/u-fire for feature requests, bug reports, and  questions
   questions@ufire.co to get in touch with someone

   This example is compatible with hardware board version 1c.

   It demonstrates the minimal amount of code to use single point calibration. 
 */

#include <ECSalinity.h>

EC_Salinity ec;

float calibrationSolution_mS = 2.0;

void setup()
{
  Serial.begin(9600);

  // set the cell-constant of your probe
  ec.setK(1.0);
  
  // Put the probe in the calibration solution for around 5 minutes. This example is for freshwater/hydroponics,
  // and uses the temperature coefficient for that (tempCoefEC) rather than for salinity (tempCoefSalinity).
  ec.calibrateProbe(calibrationSolution_mS, ec.tempCoefEC);

  // The probe will now automatically correct the readings it takes. Single point is best for when you are only
  // expecting to measure across a relatively small range. Your calibration solution should ideally be in the
  // middle of your range. 
}

void loop()
{
  ec.measureEC();
  Serial.print("mS: "); Serial.println(ec.mS);
  Serial.println("-----");
  delay(1000);
}
