#include <Arduino.h>

float K;

void calibrateK(float solutionEC, float tempC, float tempCoef)
{
  float ec;
  float conductivityAverage;
  int   i = 21;

  while (i--)
  {
    measureConductivity();
    conductivityAverage += conductivity;
    delay(250);
  }

  conductivityAverage /= 21;

  // Determine the conductivity.
  ec =  solutionEC * (1.0 + tempCoef * (tempC - 25.0));
  K  = (1000.0 / (conductivityMedian.getAverage(middleThird) * ec) * 100);
}

void setup()
{}

void loop()
{}
