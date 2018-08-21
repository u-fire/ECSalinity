/*!
   ufire.co for links to documentation, examples, and libraries
   github.com/u-fire for feature requests, bug reports, and  questions
   questions@ufire.co to get in touch with someone

    This allows you to run various functions on a command-line like interface.
    Enter `config` to see the configuration of the device. Type 'ec' to get a
    measurement in mS, 'data' to see all the data members of the class like uS,
    and S, 'temp' to read the temperature, 'sal' to use salinity parameters for
    a measurment.

    To use an unconfigured device:
     k <K constant value eg. 1.0>
     dry (while probe is dry, not needed)
     cal <calibration solution in mS eg 2.77>
     ec

    Dual Point Calibration:
     k <K constant value eg. 1.0>
     dry (while probe is dry, not needed)
     low <calibration solution in mS eg 0.5>
     high <calibration solution in mS eg 2.77>
     dp 1

    Using Temperature compensation:
     tc 1 25 <to adjust readings as if they were at 25 C>
     (recalibrate all values using `dry`, `cal`, `low`, `high`)
     ec

    Measure saltwater with a configured device:
     sal

 */

 #include <Arduino.h>
 #include "ECSalinity.h"

// Comment/uncomment the appropriate line below
// EC_Salinity EC(19, 23); // ESP32 custom pin notation
EC_Salinity EC; // Arduino pre-set pins

String buffer, cmd, p1, p2;

void config() {
  Serial.println("EC Config:");
  Serial.print("  K: "); Serial.println(EC.getK(), 4);
  Serial.print("  offset: "); Serial.println(EC.getCalibrateOffset(), 4);
  Serial.print("  dry: "); Serial.println(EC.getCalibrateDry(), 4);
  Serial.print("  dual point: "); Serial.println(EC.usingDualPoint(), 4);
  Serial.print("  low reference / read: "); Serial.print(EC.getCalibrateLowReference(), 4);
  Serial.print(" /  "); Serial.println(EC.getCalibrateLowReading(), 4);
  Serial.print("  high reference / read: "); Serial.print(EC.getCalibrateHighReference(), 4);
  Serial.print(" / "); Serial.println(EC.getCalibrateHighReading(), 4);
  Serial.print("  temp. compensation: "); Serial.println(
    EC.usingTemperatureCompensation());
  Serial.print("    constant: "); Serial.println(EC.getTempConstant());
  Serial.print("  version: "); Serial.println(EC.getVersion(), HEX);
}

void reset() {
  EC.reset();
  config();
}

void temperature() {
  if (p1.length()) {
    EC.setTemp(p1.toFloat());
  } else {
    EC.measureTemp();
  }
  Serial.print("C|F: "); Serial.print(EC.tempC);
  Serial.print(" | "); Serial.println(EC.tempF);
}

void calibrate() {
  if (p1.length()) {
    EC.calibrateProbe(p1.toFloat(), EC.tempCoefEC);
  }

  Serial.print("offset: ");
  Serial.println(EC.getCalibrateOffset(), 5);
}

void data() {
  Serial.print("S: "); Serial.println(EC.S, 4);
  Serial.print("mS: "); Serial.println(EC.mS, 4);
  Serial.print("uS: "); Serial.println(EC.uS);
  Serial.print("TDS 500|640|700: "); Serial.print(EC.PPM_500);
  Serial.print(" | "); Serial.print(EC.PPM_640);
  Serial.print(" | "); Serial.println(EC.PPM_700);
  Serial.print("salinity PSU | PPT: "); Serial.print(EC.salinityPSU, 4);
  Serial.print(" | "); Serial.println(EC.salinityPPT, 4);
  Serial.print("C|F: "); Serial.print(EC.tempC);
  Serial.print(" |  "); Serial.println(EC.tempF);
}

void low() {
  if (p1.length()) {
    EC.calibrateProbeLow(p1.toFloat(), EC.tempCoefEC);
  }

  Serial.print("low reference | read: "); Serial.print(EC.getCalibrateLowReference(), 2);
  Serial.print(" | "); Serial.println(EC.getCalibrateLowReading(), 2);
}

void high() {
  if (p1.length()) {
    EC.calibrateProbeHigh(p1.toFloat(), EC.tempCoefEC);
  }

  Serial.print("high reference | read: "); Serial.print(EC.getCalibrateHighReference(), 2);
  Serial.print(" | "); Serial.println(EC.getCalibrateHighReading(), 2);
}

void temp_comp() {
  if (p1.length()) {
    EC.useTemperatureCompensation(p1.toInt());
  }

  if (p2.length()) {
    EC.useTemperatureCompensation(p1.toInt());
    EC.setTempConstant(p2.toInt());
  }

  Serial.print("temp. compensation: ");
  Serial.println(EC.usingTemperatureCompensation());
  Serial.print("   constant: ");
  Serial.println(EC.getTempConstant());
}

void dual_point() {
  if (p1.length()) {
    EC.useDualPoint(p1.toInt());
  }

  Serial.print("dual point: ");
  Serial.println(EC.usingDualPoint());
}

void i2c() {
  if (p1.length()) {
    EC.setI2CAddress(p1.toInt());
  }
}

void k() {
  if (p1.length()) {
    EC.setK(p1.toFloat());
  }

  Serial.print("K: ");
  Serial.println(EC.getK(), 4);
}

void ec() {
  // for (;;) {
  EC.measureEC(EC.tempCoefEC, false);
  Serial.print("mS: ");
  Serial.println(EC.mS, 4);
  delay(500);

  // }
}

void sal() {
  for (;;) {
    EC.measureSalinity();
    Serial.print("salinity PSU / PPT / PPM: "); Serial.println(EC.salinityPSU, 2);
    Serial.print(" / "); Serial.print(EC.salinityPPT, 2);
    Serial.print(" / "); Serial.print(EC.salinityPPM, 2);
  }
}

void dry() {
  EC.calibrateDry();
  Serial.print("dry: ");
  Serial.println(EC.getCalibrateDry(), 4);
}

void cmd_run() {
  if ((cmd == "conf") || (cmd == "config") || (cmd == "c")) config();
  if ((cmd == "reset") || (cmd == "r")) reset();
  if ((cmd == "temp") || (cmd == "t")) temperature();
  if ((cmd == "calibrate") || (cmd == "cal")) calibrate();
  if ((cmd == "data") || (cmd == "d")) data();
  if (cmd == "low") low();
  if (cmd == "high") high();
  if (cmd == "tc") temp_comp();
  if (cmd == "dp") dual_point();
  if (cmd == "i2c") i2c();
  if (cmd == "k") k();
  if (cmd == "ec") ec();
  if (cmd == "sal") sal();
}

void cli_process() {
  while (Serial.available()) {
    char c = Serial.read();

    switch (c) {
    case '\n': // new line
      Serial.println();
      cmd = buffer.substring(0, buffer.indexOf(" ", 0)); cmd.trim();
      buffer.remove(0, buffer.indexOf(" ", 0)); buffer.trim();
      p1 = buffer.substring(0, buffer.indexOf(" ", 0)); p1.trim();
      buffer.remove(0, buffer.indexOf(" ", 0)); buffer.trim();
      p2 = buffer.substring(0, buffer.indexOf(" ", 0)); p2.trim();
      cmd_run();
      Serial.print("> ");
      buffer = "";
      break;

    case '\b': // backspace
      buffer.remove(buffer.length() - 1);
      Serial.print("\b \b");
      break;

    default: // everything else
      buffer += c;
      Serial.print(c);
    }
  }
}

void setup() {
  Serial.begin(9600);
  config();
  Serial.print("> ");
}

void loop() {
  cli_process();
}
