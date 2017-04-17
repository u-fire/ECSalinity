#include <ECSalinity.h>

EC_Salinity ec;

void setup()
{
        Serial.begin(9600);
        while (!Serial) {; }
        Serial.println();
        ec.sampleNumber = 6;
        Serial.println("Calibrating...");
        ec.calibrateProbe(2.77, ec.tempCoefEC, 10);
}

void loop()
{
        ec.measureEC(ec.tempCoefEC);
        Serial.print("mS/cm: "); Serial.println(ec.mS);
        Serial.print("PPM: "); Serial.println(ec.PPM_500);
        Serial.print("Temp C/F: "); Serial.print(ec.tempC); Serial.print("/"); Serial.println(ec.tempF);
        Serial.println("-----");
        delay(10000);
}
