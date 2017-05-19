#include <ECSalinity.h>

EC_Salinity ec;

void setup()
{
        Serial.begin(9600);
        Serial.println("Calibrating...");
        ec.calibrateProbe(2.77, ec.tempCoefEC);
}

void loop()
{
        ec.measureTemp();
        ec.measureEC(ec.tempCoefEC);
        Serial.print("mS/cm: "); Serial.println(ec.mS);
        Serial.print("PPM: "); Serial.println(ec.PPM_500);
        Serial.print("Temp C/F: "); Serial.print(ec.tempC); Serial.print("/"); Serial.println(ec.tempF);
        Serial.println("-----");
        delay(10000);
}
