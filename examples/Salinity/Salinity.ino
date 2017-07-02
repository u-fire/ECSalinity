// For more information and documentation, visit ufire.co

#include <ECSalinity.h>

EC_Salinity ec;
float calibrationSolution_mS = 53.0;

void setup()
{
  Serial.begin(9600);
  Serial.println("Calibrating...");
  ec.setK(10.0);
  ec.calibrateProbe(calibrationSolution_mS, ec.tempCoefSalinity);
  Serial.print("  offset: "); Serial.println(ec.getCalibrateOffset());
}

void loop()
{
  ec.measureTemp();
  ec.measureSalinity();
  Serial.print("PPT / PSU: "); Serial.print(ec.salinityPPT);
  Serial.print(" / ");
  Serial.println(ec.salinityPSU);
  Serial.print("Temp C/F: "); Serial.print(ec.tempC); Serial.print("/");
  Serial.println(ec.tempF);
  Serial.println("-----");
  delay(10000);
}
