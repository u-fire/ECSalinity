/*!
   ufire.co for links to documentation, examples, and libraries
   github.com/u-fire for feature requests, bug reports, and  questions
   questions@ufire.co to get in touch with someone

   This example is compatible with hardware board version 2.

   It show's a user interactive dual point calibration routine.
 */

#include <ECSalinity.h>

EC_Salinity ec;

void setup()
{
  Serial.begin(9600);

  // reset the configuration for this example
  ec.reset();

  // if temperature compensation will be used, turn it on now
  ec.useTemperatureCompensation(true);

  // For hydroponics or pool monitoring:
  // use the EC value listed under the current temperature to calibrate
  // eg. if the temperature of the calibration solution is 20 C, use the
  // EC value listed for that temperature
  ec.calibrateEC(2.0);
}

void loop()
{
  ec.measureEC();
  Serial.print("mS/cm: ");
  Serial.println(ec.mS);
  delay(1000);
}
