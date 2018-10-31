/*!
   ufire.co for links to documentation, examples, and libraries
   github.com/u-fire for feature requests, bug reports, and  questions
   questions@ufire.co to get in touch with someone

   This example is compatible with hardware board version 2.

   It demonstrates the minimal amount of code and setup to take a reading.

   There is a slight difference in the SAMD21 core. The only difference
   between this example and all the others is that `Wire.begin();` is
   called in setup().
 */

#include <ECSalinity.h>

EC_Salinity ec;

void setup()
{
  Wire.begin();
  Serial.begin(9600);
}

void loop()
{
  ec.measureEC();
  Serial.print("mS/cm: "); Serial.println(ec.mS);
  Serial.print("TDS: "); Serial.println(ec.PPM_500);
  Serial.println("-----");
  delay(1000);
}
