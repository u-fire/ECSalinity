#include <ECSalinity.h>

EC_Salinity ec;

void setup()
{
        Serial.begin(9600);
        while (!Serial) {; }
        Serial.println();
        ec.sampleNumber = 6;
        Serial.println("Calibrating...");
        ec.calibrateProbe(53, ec.tempCoefSalinity, 10);
}

void loop()
{
        ec.measureEC(ec.tempCoefSalinity);
        Serial.print("mS/cm: "); Serial.println(ec.mS);
        Serial.print("mS/cm: "); Serial.println(ec.mS);
        Serial.print("Salinity PSU/PPT: "); Serial.print(ec.salinityPSU); Serial.print("/"); Serial.println(ec.salinityPPT);
        Serial.print("Temp C/F: "); Serial.print(ec.tempC); Serial.print("/"); Serial.println(ec.tempF);
        Serial.println("-----");
        delay(10000);
}
