// Copyright (c) 2017 Justin Decker

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

/*!
   \file ECSalinity.h
   \brief EC_Salinity Class Implementation
 */

#ifndef EC_SALINITY_H
#define EC_SALINITY_H

#include <math.h>

#if defined(PARTICLE)
#include "application.h"
#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))
#else // if defined(PARTICLE)
#include <Arduino.h>
#include <Wire.h>
#endif // if defined(PARTICLE)

#define EC_SALINITY 0x3c
#define EC_MEASURE_EC 80
#define EC_MEASURE_SW 40
#define EC_MEASURE_TEMP 20
#define EC_CALIBRATE_EC 10
#define EC_CALIBRATE_SW 8
#define EC_I2C 4
#define EC_READ 2
#define EC_WRITE 1

#define EC_VERSION_REGISTER 0            /*!< version register */
#define EC_FW_VERSION_REGISTER 1         /*!< firmware version register */
#define EC_MS_REGISTER 2                 /*!< mS register */
#define EC_SALINITY_PSU 6                /*!< salinity register */
#define EC_TEMP_REGISTER 10              /*!< temperature in C register */
#define EC_RAW_REGISTER 14               /*!< raw count register */
#define EC_SOLUTION_REGISTER 18          /*!< calibration solution register */
#define EC_CALIBRATE_EC_REGISTER 22      /*!< temperatue coefficient register */
#define EC_CALIBRATE_SW_REGISTER 26      /*!< reference low register */
#define EC_TEMP_COMPENSATION_REGISTER 30 /*!< temperature compensation register */
#define EC_BUFFER_REGISTER 34            /*!< buffer register */
#define EC_CONFIG_REGISTER 38            /*!< config register */
#define EC_TASK_REGISTER 39              /*!< task register */

#define EC_EC_MEASUREMENT_TIME 250 /*!< delay between EC measurements */
#define EC_TEMP_MEASURE_TIME 750   /*!< delay for temperature measurement */

#define EC_TEMP_COMPENSATION_CONFIG_BIT 0 /*!< temperature compensation config bit */

class EC_Salinity /*! EC Salinity Class */
{
public:
  float S;           /*!< EC in Siemens */
  float mS;          /*!< EC in milli-Siemens */
  float uS;          /*!< EC in micro-Siemens */
  float raw;         /*!< raw measurement */
  long PPM_500;      /*!< Parts per million using 500 as a multiplier */
  long PPM_640;      /*!< Parts per million using 640 as a multiplier */
  long PPM_700;      /*!< Parts per million using 700 as a multiplier */
  float salinityPSU; /*!< Salinity measured practical salinity units */
  float tempC;       /*!< Temperature in C */
  float tempF;       /*!< Temperature in F */
  EC_Salinity(uint8_t i2c_address);
  EC_Salinity();
#ifdef ESP32
  EC_Salinity(uint8_t sda,
              uint8_t scl,
              uint8_t i2c_address);
  EC_Salinity(uint8_t sda,
              uint8_t scl);
#endif // ifndef ESP32
  // measurements
  float measureEC();
  float measureSW();
  float measureEC(bool newTemp);
  float measureSW(bool newTemp);
  float measureTemp();

  // calibration
  void calibrateEC(float solutionEC);
  float getCalibrationEC();
  void calibrateSW(float solutionSW);
  float getCalibrationSW();

  // temperature
  void setTemp(float temp_C);
  void setTempConstant(float b);
  float getTempConstant();
  void useTemperatureCompensation(bool b);
  bool usingTemperatureCompensation();

  // utility
  void reset();
  void setI2CAddress(uint8_t i2cAddress);
  bool connected();
  void writeEEPROM(uint8_t address,
                   float value);
  float readEEPROM(uint8_t address);

  // version
  uint8_t getVersion();
  uint8_t getFirmware();

private:
  uint8_t _address;
  float _measure(bool EC,
                 bool newTemp);
  void _change_register(uint8_t register);
  void _send_command(uint8_t command);
  void _write_register(uint8_t reg,
                       float f);
  void _write_byte(uint8_t reg,
                   uint8_t val);
  float _read_register(uint8_t reg);
  uint8_t _read_byte(uint8_t reg);
};

#endif // ifndef EC_SALINITY_H
