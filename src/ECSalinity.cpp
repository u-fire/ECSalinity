// Copyright (c) 2018 Justin Decker

//
// MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "ECSalinity.h"

EC_Salinity::EC_Salinity(uint8_t i2c_address)
{
  _address = i2c_address;
#ifndef ARDUINO_SAMD_ZERO
  Wire.begin();
#endif // ifndef ARDUINO_SAMD_ZERO
}

EC_Salinity::EC_Salinity()
{
  _address = EC_SALINITY;
#ifndef ARDUINO_SAMD_ZERO
  Wire.begin();
#endif // ifndef ARDUINO_SAMD_ZERO
}

#ifdef ESP32
EC_Salinity::EC_Salinity(uint8_t sda, uint8_t scl, uint8_t i2c_address)
{
  _address = i2c_address;
  Wire.begin(sda, scl, 100000);
}

EC_Salinity::EC_Salinity(uint8_t sda, uint8_t scl)
{
  _address = EC_SALINITY;
  Wire.begin(sda, scl, 100000);
}

#endif // ifndef ESP32

// measurements
float EC_Salinity::_measure(bool EC, bool newTemp)
{
  if (newTemp)
  {
    measureTemp();
  }

  if (EC)
  {
    _send_command(EC_MEASURE_EC);
  }
  else
  {
    _send_command(EC_MEASURE_SW);
  }

  delay(EC_EC_MEASUREMENT_TIME);
  mS  = _read_register(EC_MS_REGISTER);
  raw = _read_register(EC_RAW_REGISTER);

  if (raw == 0.0)
  {
    mS = NAN; // make it NaN so the following statement will -1 everything
  }

  if (mS == mS)
  {
    PPM_500 = mS * 500;
    PPM_640 = mS * 640;
    PPM_700 = mS * 700;
    uS      = mS * 1000;
    S       = mS / 1000;

    salinityPSU = _read_register(EC_SALINITY_PSU);
  }
  else
  {
    mS          = -1;
    PPM_500     = -1;
    PPM_640     = -1;
    PPM_700     = -1;
    uS          = -1;
    S           = -1;
    salinityPSU = -1;
  }

  return mS;
}

float EC_Salinity::measureEC()
{
  return _measure(true, usingTemperatureCompensation());
}

float EC_Salinity::measureEC(bool newTemp)
{
  return _measure(true, newTemp);
}

float EC_Salinity::measureSW()
{
  _measure(false, usingTemperatureCompensation());
  return salinityPSU;
}

float EC_Salinity::measureSW(bool newTemp)
{
  _measure(false, newTemp);
  return salinityPSU;
}

float EC_Salinity::measureTemp()
{
  _send_command(EC_MEASURE_TEMP);
  delay(EC_TEMP_MEASURE_TIME);
  tempC = _read_register(EC_TEMP_REGISTER);
  if (tempC == -127.0)
  {
    tempF = -127;
  }
  else
  {
    tempF = ((tempC * 9) / 5) + 32;
  }
  return tempC;
}

// calibration
void EC_Salinity::calibrateEC(float solutionEC)
{
  _write_register(EC_SOLUTION_REGISTER, solutionEC);
  _send_command(EC_CALIBRATE_EC);
  delay(EC_EC_MEASUREMENT_TIME);
}

float EC_Salinity::getCalibrationEC()
{
  return _read_register(EC_CALIBRATE_EC_REGISTER);
}

void EC_Salinity::calibrateSW(float solutionSW)
{
  _write_register(EC_SOLUTION_REGISTER, solutionSW);
  _send_command(EC_CALIBRATE_SW);
  delay(EC_EC_MEASUREMENT_TIME);
}

float EC_Salinity::getCalibrationSW()
{
  return _read_register(EC_CALIBRATE_SW_REGISTER);
}

// temperature
void EC_Salinity::setTemp(float temp_C)
{
  _write_register(EC_TEMP_REGISTER, temp_C);
  tempC = temp_C;
  tempF = ((tempC * 9) / 5) + 32;
}

void EC_Salinity::setTempConstant(float b)
{
  _write_byte(EC_TEMP_COMPENSATION_REGISTER, b);
}

float EC_Salinity::getTempConstant()
{
  return _read_register(EC_TEMP_COMPENSATION_REGISTER);
}

void EC_Salinity::useTemperatureCompensation(bool b)
{
  uint8_t retval;
  uint8_t config = _read_byte(EC_CONFIG_REGISTER);

  if (b)
  {
    retval = bitSet(config, EC_TEMP_COMPENSATION_CONFIG_BIT);
  }
  else
  {
    retval = bitClear(config, EC_TEMP_COMPENSATION_CONFIG_BIT);
  }

  _write_byte(EC_CONFIG_REGISTER, retval);
}

bool EC_Salinity::usingTemperatureCompensation()
{
  uint8_t retval;

  retval = _read_byte(EC_CONFIG_REGISTER);
  return (retval >> EC_TEMP_COMPENSATION_CONFIG_BIT) & 0x01;
}

// utilities
uint8_t EC_Salinity::getVersion()
{
  return _read_byte(EC_VERSION_REGISTER);
}

uint8_t EC_Salinity::getFirmware()
{
  return _read_byte(EC_FW_VERSION_REGISTER);
}

void EC_Salinity::reset()
{
  _write_register(EC_CALIBRATE_EC_REGISTER, NAN);
  delay(10);
  _write_register(EC_CALIBRATE_SW_REGISTER, NAN);
  delay(10);
  setTempConstant(25);
  delay(10);
  useTemperatureCompensation(false);
}

void EC_Salinity::setI2CAddress(uint8_t i2cAddress)
{
  _write_register(EC_BUFFER_REGISTER, i2cAddress);
  _send_command(EC_I2C);
  _address = i2cAddress;
}

bool EC_Salinity::connected()
{
  uint8_t retval;

  retval = _read_byte(EC_VERSION_REGISTER);
  if (retval != 0xFF)
  {
    return true;
  }
  else
  {
    return false;
  }
}

float EC_Salinity::readEEPROM(uint8_t address)
{
  _write_register(EC_SOLUTION_REGISTER, address);
  _send_command(EC_READ);
  return _read_register(EC_BUFFER_REGISTER);
}

void EC_Salinity::writeEEPROM(uint8_t address, float value)
{
  _write_register(EC_SOLUTION_REGISTER, address);
  _write_register(EC_BUFFER_REGISTER,   value);
  _send_command(EC_WRITE);
}

//////////////////////////////////////////////////

void EC_Salinity::_change_register(uint8_t r)
{
  Wire.beginTransmission(_address);
  Wire.write(r);
  Wire.endTransmission();
  delay(10);
}

void EC_Salinity::_send_command(uint8_t command)
{
  Wire.beginTransmission(_address);
  Wire.write(EC_TASK_REGISTER);
  Wire.write(command);
  Wire.endTransmission();
  delay(10);
}

void EC_Salinity::_write_register(uint8_t reg, float f)
{
  uint8_t b[5];
  float   f_val = f;

  b[0] = reg;
  b[1] = *((uint8_t *)&f_val);
  b[2] = *((uint8_t *)&f_val + 1);
  b[3] = *((uint8_t *)&f_val + 2);
  b[4] = *((uint8_t *)&f_val + 3);
  Wire.beginTransmission(_address);
  Wire.write(b, 5);
  Wire.endTransmission();
  delay(10);
}

float EC_Salinity::_read_register(uint8_t reg)
{
  float retval;

  _change_register(reg);
  Wire.requestFrom(_address, (uint8_t)1);
  *((uint8_t *)&retval) = Wire.read();
  Wire.requestFrom(_address, (uint8_t)1);
  *((uint8_t *)&retval + 1) = Wire.read();
  Wire.requestFrom(_address, (uint8_t)1);
  *((uint8_t *)&retval + 2) = Wire.read();
  Wire.requestFrom(_address, (uint8_t)1);
  *((uint8_t *)&retval + 3) = Wire.read();
  delay(10);
  return retval;
}

void EC_Salinity::_write_byte(uint8_t reg, uint8_t val)
{
  uint8_t b[5];

  b[0] = reg;
  b[1] = val;
  Wire.beginTransmission(_address);
  Wire.write(b, 2);
  Wire.endTransmission();
  delay(10);
}

uint8_t EC_Salinity::_read_byte(uint8_t reg)
{
  uint8_t retval;

  _change_register(reg);
  Wire.requestFrom(_address, (uint8_t)1);
  retval = Wire.read();
  delay(10);
  return retval;
}
