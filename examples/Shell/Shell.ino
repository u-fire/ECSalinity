/*
  ufire.co for links to documentation, examples, and libraries
  github.com/u-fire for feature requests, bug reports, and  questions
  questions@ufire.co to get in touch with someone

  This example is compatible with hardware board version 2.

  This allows you to run various functions on a command-line like interface.
  Enter `config` to see the configuration of the device. Type 'ec' to get a
  measurement in mS, 'data' to see all the data members of the class like uS,
  and S, 'temp' to read the temperature, 'sal' to use salinity parameters for
  a measurment.

  Calibration:
    low <calibration solution in mS eg 0.7>
    high <calibration solution in mS eg 2.0>
    <measurements will automatically use points as soon as both low and high are received>

  Using Temperature compensation with attached temperature sensor:
    tc 1 25 <to adjust readings as if they were at 25 C>
    ec
    tc 0 <to disable compensation>

  Set a temperature to use
    t 20 <to use 20 C as the temperature rather than using the attached temp. sensor>

  Measure saltwater with a configured device:
    sal
 */

#include <Arduino.h>
#include "ECSalinity.h"
#include "EC_Curve.h"

EC_Salinity EC; // sda, scl
EC_Curve EC_poly;

String buffer, cmd, p1, p2;

void config()
{
  Serial.print("EC Interface: ");
  Serial.println(EC.connected() ? "connected" : "*disconnected*");
  Serial.println("  dual point: ");
  Serial.print("    low reference / read: ");
  Serial.print(EC.getCalibrateLowReference(), 4);
  Serial.print(" |  ");
  Serial.println(EC.getCalibrateLowReading(), 4);
  Serial.print("    high reference / read: ");
  Serial.print(EC.getCalibrateHighReference(), 4);
  Serial.print(" | ");
  Serial.println(EC.getCalibrateHighReading(), 4);
  Serial.print("  temp. compensation: ");
  Serial.println(EC.usingTemperatureCompensation() ? "yes" : "no");
  Serial.print("    constant: ");
  Serial.println(EC.getTempConstant());
  Serial.print("  version: ");
  Serial.print(EC.getVersion(), HEX);
  Serial.print(".");
  Serial.println(EC.getFirmware(), HEX);
}

void reset()
{
  EC.reset();
  config();
}

void temperature()
{
  if (p1.length())
  {
    EC.setTemp(p1.toFloat());
  }
  else
  {
    EC.measureTemp();
  }
  Serial.print("C|F: ");
  Serial.print(EC.tempC);
  Serial.print(" | ");
  Serial.println(EC.tempF);
}

void data()
{
  Serial.print("raw: ");
  Serial.println(EC.raw);
  Serial.print(" S/cm: ");
  Serial.println(EC.S, 4);
  Serial.print("mS/cm: ");
  Serial.println(EC.mS, 4);
  Serial.print("uS/cm: ");
  Serial.println(EC.uS);
  Serial.print("TDS PPM 500|640|700: ");
  Serial.print(EC.PPM_500);
  Serial.print(" | ");
  Serial.print(EC.PPM_640);
  Serial.print(" | ");
  Serial.println(EC.PPM_700);
  Serial.print("salinity PSU: ");
  Serial.println(EC.salinityPSU, 4);
  Serial.print("C|F: ");
  Serial.print(EC.tempC);
  Serial.print(" |  ");
  Serial.println(EC.tempF);
}

void low()
{
  if (p1.length())
  {
    EC.calibrateProbeLow(p1.toFloat(), EC.tempCoefEC);
  }

  Serial.print("low reference | read: ");
  Serial.print(EC.getCalibrateLowReference(), 2);
  Serial.print(" | ");
  Serial.println(EC.getCalibrateLowReading(), 2);
}

void high()
{
  if (p1.length())
  {
    EC.calibrateProbeHigh(p1.toFloat(), EC.tempCoefEC);
  }

  Serial.print("high reference | read: ");
  Serial.print(EC.getCalibrateHighReference(), 2);
  Serial.print(" | ");
  Serial.println(EC.getCalibrateHighReading(), 2);
}

void temp_comp()
{
  if (p1.length())
  {
    EC.useTemperatureCompensation(p1.toInt());
  }

  if (p2.length())
  {
    EC.useTemperatureCompensation(p1.toInt());
    EC.setTempConstant(p2.toInt());
  }

  Serial.print("temp. compensation: ");
  Serial.println(EC.usingTemperatureCompensation());
  Serial.print("   constant: ");
  Serial.println(EC.getTempConstant());
}

void i2c()
{
  if (p1.length())
  {
    EC.setI2CAddress(p1.toInt());
  }
}

void ec()
{
  if (p1.length())
  {
    while (Serial.available() == 0)
    {
      EC.measureEC(EC.tempCoefEC, false);
      Serial.print("mS/cm: ");
      Serial.println(EC.mS, 4);
      delay(p1.toInt());
    }
  }
  else
  {
    EC.measureEC(EC.tempCoefEC, false);
    Serial.print("mS/cm: ");
    Serial.println(EC.mS, 4);
  }
}

void er()
{
  if (p1.length())
  {
    while (Serial.available() == 0)
    {
      EC_poly.measureEC(EC.tempCoefEC, false);
      Serial.print("mS: ");
      Serial.println(EC_poly.mS, 4);
      delay(p1.toInt());
    }
  }
  else
  {
    EC_poly.measureEC(EC.tempCoefEC, false);
    Serial.print("mS: ");
    Serial.println(EC_poly.mS, 4);
  }
}

void raw()
{
  if (p1.length())
  {
    while (Serial.available() == 0)
    {
      EC.measureEC(EC.tempCoefEC, false);
      Serial.print("raw: ");
      Serial.println(EC.raw);
      delay(p1.toInt());
    }
  }
  else
  {
    EC.measureEC(EC.tempCoefEC, false);
    Serial.print("raw: ");
    Serial.println(EC.raw);
  }
}

void sal()
{
  if (p1.length())
  {
    while (Serial.available() == 0)
    {
      EC.measureEC(EC.tempCoefSalinity, false);
      Serial.print("salinity PSU: ");
      Serial.println(EC.salinityPSU, 2);
      delay(p1.toInt());
    }
  }
  else
  {
    EC.measureEC(EC.tempCoefSalinity, false);
    Serial.print("salinity PSU: ");
    Serial.println(EC.salinityPSU, 2);
  }
}

void cmd_run()
{
  if ((cmd == "conf") || (cmd == "config") || (cmd == "c"))
    config();
  if ((cmd == "reset") || (cmd == "r"))
    reset();
  if ((cmd == "temp") || (cmd == "t"))
    temperature();
  if ((cmd == "data") || (cmd == "d"))
    data();
  if (cmd == "low")
    low();
  if (cmd == "high")
    high();
  if (cmd == "tc")
    temp_comp();
  if (cmd == "i2c")
    i2c();
  if (cmd == "ec")
    ec();
  if (cmd == "sal")
    sal();
  if (cmd == "raw")
    raw();
  if (cmd == "er")
    er();
}

void cli_process()
{
  while (Serial.available())
  {
    char c = Serial.read();

    switch (c)
    {
    case '\n': // new line
      Serial.println();
      cmd = buffer.substring(0, buffer.indexOf(" ", 0));
      cmd.trim();
      buffer.remove(0, buffer.indexOf(" ", 0));
      buffer.trim();
      p1 = buffer.substring(0, buffer.indexOf(" ", 0));
      p1.trim();
      buffer.remove(0, buffer.indexOf(" ", 0));
      buffer.trim();
      p2 = buffer.substring(0, buffer.indexOf(" ", 0));
      p2.trim();
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

void setup()
{
  Wire.begin();
  Serial.begin(9600);
  config();
  Serial.print("> ");
}

void loop()
{
  cli_process();
}
