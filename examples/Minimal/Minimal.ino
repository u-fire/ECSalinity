/*!
   ufire.co for links to documentation, examples, and libraries
   github.com/u-fire for feature requests, bug reports, and  questions
   questions@ufire.co to get in touch with someone

   This example is compatible with hardware board version 1c.

   It demonstrates the minimal amount of code and setup to take a reading. 
 */

#include <ECSalinity.h>

EC_Salinity ec;

void setup()
{
  Serial.begin(9600);

  // the minimal configuration is setting the cell constant, referred to as K
  ec.setK(1.0);
}

void loop()
{
  ec.measureEC();
  Serial.print("mS: "); Serial.println(ec.mS);
  Serial.print("TDS: "); Serial.println(ec.PPM_500);
  Serial.println("-----");
  delay(1000);
}
