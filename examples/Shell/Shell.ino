/*
   ufire.co for links to documentation, examples, and libraries
   github.com/u-fire for feature requests, bug reports, and  questions
   questions@ufire.co to get in touch with someone

   This example is compatible with hardware board version 2.

   This allows you to run various functions on a command-line like interface.
   Enter:
    `config` to see the configuration of the device.
    'ec' to get a measurement in mS
    'sw' to measure seawater
    'data' to see all the data  members of the class like uS, and S
    'temp' to read the temperature
    'reset' to reset all the configuration stored in the device

   Using Temperature compensation with attached temperature sensor:
    tc 1 25 <to adjust readings as if they were at 25 C>
    tc 0 <to disable compensation>

   Calibration:
    cal-ec 2.0 <to calibrate for the hydroponic or pool range>
    cal-sw 53.0 <to calibrate for seawater>

   Set a temperature to use:
    t 20 <to use 20 C as the temperature rather than using the attached temp. sensor>

   Measure Temperature:
    temp

   Measure EC:
    ec
    ec x <take an EC measurement every x milliseconds>

   Measure saltwater:
    sw
    sw  x <take a SW measurement every x milliseconds>

   Display results in PPM 500:
    tds
    tds x <take a measurement every x milliseconds>

   Change the I2C address:
    i2c 3d <address is a interpreted as a hex number without the 0x>

   Read/Write custom EEPROM data:
    read 300 <returns a float stored at address 300>
    write 300 123.4 <writes 123.4 at address 300>
 */

#include <Arduino.h>
#include "ECSalinity.h"

#ifdef ESP32
EC_Salinity EC(19, 23); // sda, scl
#else // ifdef ESP32
EC_Salinity EC;
#endif // ifdef ESP32

String buffer, cmd, p1, p2;

void config()
{
  Serial.print("EC Interface: ");
  Serial.println(EC.connected() ? "connected" : "*disconnected*");
  Serial.println("calibration:");
  Serial.print("  EC: ");
  Serial.println(EC.getCalibrationEC());
  Serial.print("  SW: ");
  Serial.println(EC.getCalibrationSW());
  Serial.print("temp. compensation: ");
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

void cal_ec()
{
  if (p1.length())
  {
    EC.calibrateEC(p1.toFloat());
  }

  Serial.print("calibration EC: ");
  Serial.println(EC.getCalibrationEC(), 2);
}

void cal_sw()
{
  if (p1.length())
  {
    EC.calibrateSW(p1.toFloat());
  }

  Serial.print("calibration SW: ");
  Serial.println(EC.getCalibrationSW(), 2);
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
    EC.setTempConstant(p2.toFloat());
  }

  Serial.print("temp. compensation: ");
  Serial.println(EC.usingTemperatureCompensation() ? "yes" : "no");
  Serial.print("   constant: ");
  Serial.println(EC.getTempConstant());
}

void i2c()
{
  if (p1.length())
  {
    EC.setI2CAddress(strtoul(p1.c_str(), 0, 16));
  }
}

void ec()
{
  if (p1.length())
  {
    while (Serial.available() == 0)
    {
      EC.measureEC(false);
      Serial.print("mS/cm: ");
      Serial.println(EC.mS, 4);
      delay(p1.toInt());
    }
  }
  else
  {
    EC.measureEC(false);
    Serial.print("mS/cm: ");
    Serial.println(EC.mS, 4);
  }
}

void tds()
{
  if (p1.length())
  {
    while (Serial.available() == 0)
    {
      EC.measureEC(false);
      Serial.print("tds: ");
      Serial.println(EC.PPM_500);
      delay(p1.toInt());
    }
  }
  else
  {
    EC.measureEC(false);
    Serial.print("tds: ");
    Serial.println(EC.PPM_500);
  }
}

void raw()
{
  if (p1.length())
  {
    while (Serial.available() == 0)
    {
      EC.measureEC(false);
      Serial.print("raw: ");
      Serial.println(EC.raw);
      delay(p1.toInt());
    }
  }
  else
  {
    EC.measureEC(false);
    Serial.print("raw: ");
    Serial.println(EC.raw);
  }
}

void sw()
{
  if (p1.length())
  {
    while (Serial.available() == 0)
    {
      EC.measureSW(false);
      Serial.print("salinity PSU: ");
      Serial.println(EC.salinityPSU, 2);
      delay(p1.toInt());
    }
  }
  else
  {
    EC.measureSW(false);
    Serial.print("salinity PSU: ");
    Serial.println(EC.salinityPSU, 2);
  }
}

void read()
{
  if (p1.length())
  {
    Serial.println(EC.readEEPROM(p1.toFloat()));
  }
}

void write()
{
  if (p1.length())
  {
    EC.writeEEPROM(p1.toInt(), p2.toFloat());
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
  if (cmd == "tc")
    temp_comp();
  if (cmd == "i2c")
    i2c();
  if (cmd == "ec")
    ec();
  if (cmd == "sw")
    sw();
  if (cmd == "raw")
    raw();
  if (cmd == "cal-ec")
    cal_ec();
  if (cmd == "cal-sw")
    cal_sw();
  if (cmd == "read")
    read();
  if (cmd == "write")
    write();
  if (cmd == "tds")
    tds();
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
