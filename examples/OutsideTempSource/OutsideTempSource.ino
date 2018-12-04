/*!
   ufire.co for links to documentation, examples, and libraries
   github.com/u-fire for feature requests, bug reports, and  questions
   questions@ufire.co to get in touch with someone

   This example is compatible with hardware board version 2.

   It demonstrates the minimal amount of code and setup to take a reading
   using temperature compensation and an outside source of temperature.
 */

#include <ECSalinity.h>

EC_Salinity ec;

// This value will come from some other sensor
float actualTemp = 20.2;

void setup()
{
  Serial.begin(9600);

  // tell the device to use temperature compensation
  ec.useTemperatureCompensation(true);

  // the temperature to adjust the readings to
  // the factor-set default is 25
  ec.setTempConstant(25);

  // the actual temperature of the solution
  ec.setTemp(actualTemp);
}

void loop()
{
  ec.measureEC(false); // take a new temperature reading = false
  Serial.print("mS/cm: ");
  Serial.println(ec.mS);
  Serial.println("-----");
  delay(1000);
}
