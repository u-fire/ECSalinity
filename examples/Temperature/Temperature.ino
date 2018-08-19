/*!
   ufire.co for links to documentation, examples, and libraries
   github.com/u-fire for feature requests, bug reports, and  questions
   questions@ufire.co to get in touch with someone

   This example is compatible with hardware board version 1c.

   It demonstrates the minimal amount of code to take a temperature reading. 
 */

#include <ECSalinity.h>

EC_Salinity ec;

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  ec.measureTemp();
  Serial.print("Temp C/F: "); 
  Serial.print(ec.tempC); 
  Serial.print("/");
  Serial.println(ec.tempF);
  Serial.println("-----");
  delay(1000);
}
