#include <ECSalinity.h>

EC_Salinity ec;

void setup()
{
        Serial.begin(9600);
        if (ec.getK() == 0)
        {
          Serial.println("No calibration settings. Place probe in solution then press any key");
          while(Serial.available() == 0){} Serial.read();
          Serial.println("Calibrating K...");
          ec.calibrateProbe(53, ec.tempCoefSalinity);
          Serial.print("K: "); Serial.println(ec.getK());

          Serial.println("Clean probe and place in the low reference solution then press any key");
          while(Serial.available() == 0){} Serial.read();
          Serial.println("Calibrating low...");
          ec.calibrateProbeLow(50, ec.tempCoefSalinity);
          Serial.print("Reference low set at: "); Serial.println(ec.getCalibrateLow());

          Serial.println("Clean probe and place in the high reference solution then press any key");
          while(Serial.available() == 0){} Serial.read();
          Serial.println("Calibrating high...");
          ec.calibrateProbeHigh(58, ec.tempCoefSalinity);
          Serial.print("Reference high set at: "); Serial.println(ec.getCalibrateHigh());

          ec.useDualPoint(true);
        }
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
