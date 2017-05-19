#include <ECSalinity.h>

EC_Salinity ec;

void setup()
{
        Serial.begin(9600);
        Serial.println("Calibrating...");
        ec.calibrateProbe(53, ec.tempCoefSalinity);
}

void loop()
{
        ec.measureTemp();
        ec.measureEC(ec.tempCoefSalinity);
        Serial.print("mS/cm: "); Serial.println(ec.mS);
        Serial.print("Salinity PSU/PPT: "); Serial.print(ec.salinityPSU); Serial.print("/"); Serial.println(ec.salinityPPT);
        Serial.print("Temp C/F: "); Serial.print(ec.tempC); Serial.print("/"); Serial.println(ec.tempF);
        Serial.println("-----");
}
