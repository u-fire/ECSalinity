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
  ec.reset();

  // for hydroponics or pool monitoring
  ec.calibrateEC(2.0);
}

void loop()
{
  ec.measureEC();
  Serial.print("mS/cm: ");
  Serial.println(ec.mS);
  delay(1000);
}
