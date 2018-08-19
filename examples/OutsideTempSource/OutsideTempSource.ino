/*!
   ufire.co for links to documentation, examples, and libraries
   github.com/u-fire for feature requests, bug reports, and  questions
   questions@ufire.co to get in touch with someone

   This example is compatible with hardware board version 1c.

   It demonstrates the minimal amount of code and setup to take a reading
   using temperature compensation and an outside source of temperature.
 */

#include <ECSalinity.h>

EC_Salinity ec;

float actualTemp = 20.2;

void setup()
{
  Serial.begin(9600);

  // set the cell-constant
  ec.setK(1.0);

  // tell the device to use temperature compensation
  ec.useTemperatureCompensation(true);

  // the temperature to adjust the readings to
  ec.setTempConstant(25);

  // the actual temperature of the solution, as provided by some other source of temperature
  // other than the sensor attached to the board. This allows you to only have one temperature
  // sensor attached to the controller. 
  ec.setTemp(actualTemp);
}

void loop()
{
  // the parameters mean to use the freshwater/hydroponics temperature coefficient and to not
  // take a temperatur reading from the sensor on the board.
  ec.measureEC(ec.tempCoefEC, false);
  Serial.print("mS: "); Serial.println(ec.mS);
  Serial.println("-----");
  delay(1000);
}
