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
#include <Arduino.h>
#include <Wire.h>

#define EC_SALINITY 0x3C                  /*!< EC Salinity probe I2C address */
#define EC_MEASURE_EC 80                  /*!< Command to start an EC measure */
#define EC_MEASURE_TEMP 40                /*!< Command to measure temperature */
#define EC_CALIBRATE_PROBE 20             /*!< Command to calibrate the probe */
#define EC_CALIBRATE_LOW 10               /*!< Command to calibrate the low point of the probe */
#define EC_CALIBRATE_HIGH 8               /*!< Command to calibrate the high point of the probe */

#define EC_VERSION_REGISTER 0             /*!< version register */
#define EC_MS_REGISTER 1                  /*!< mS register */
#define EC_TEMP_REGISTER 5                /*!< temperature in C register */
#define EC_K_REGISTER 9                   /*!< cell constant register */
#define EC_SOLUTION_REGISTER 13           /*!< calibration solution register */
#define EC_TEMPCOEF_REGISTER 17           /*!< temperatue coefficient register */
#define EC_CALIBRATE_REFHIGH_REGISTER 21  /*!< reference low register */
#define EC_CALIBRATE_REFLOW_REGISTER 25   /*!< reference high register */
#define EC_CALIBRATE_READHIGH_REGISTER 29 /*!< reading low register */
#define EC_CALIBRATE_READLOW_REGISTER 33  /*!< reading high register */
#define EC_CALIBRATE_OFFSET_REGISTER 37   /*!< caliration offset */
#define EC_SALINITY_PSU 41                /*!< Salinity register */
#define EC_TEMP_COMPENSATION_REGISTER 45  /*!< temperature compensation register */
#define EC_ACCURACY_REGISTER 46           /*!< accuracy register */
#define EC_CONFIG_REGISTER 47             /*!< config register */
#define EC_TASK_REGISTER 48               /*!< task register */

#define EC_EC_MEASURMENT_TIME 10          /*!< delay between EC measurements */
#define EC_TEMP_MEASURE_TIME 750          /*!< delay for temperature measurement */

#define EC_DUALPOINT_CONFIG_BIT 0         /*!< dual point config bit */
#define EC_TEMP_COMPENSATION_CONFIG_BIT 1 /*!< temperature compensation config bit */

#define PSU_TO_PPT_CONVERSION 1.004715    /*!< conversion factor for PSU to PPT */

class EC_Salinity                         /*! EC Salinity Class */
{
public:

  float S;                             /*!< EC in Siemens */
  float mS;                            /*!< EC in milli-Siemens */
  float uS;                            /*!< EC in micro-Siemens */
  long  PPM_500;                       /*!< Parts per million using 500 as a multiplier */
  long  PPM_640;                       /*!< Parts per million using 640 as a multiplier */
  long  PPM_700;                       /*!< Parts per million using 700 as a multiplier */
  float salinityPSU;                   /*!< Salinity measured practical salinity units */
  float salinityPPT;                   /*!< Salinity measured parts per thousand */
  float salinityPPM;                   /*!< Salinity measured parts per million */
  float tempC;                         /*!< Temperature in C */
  float tempF;                         /*!< Temperature in F */
  static const float tempCoefEC;       /*!< Temperature compensation coefficient for EC measurement */
  static const float tempCoefSalinity; /*!< Temperature compensation coefficient for salinity measurement */
  EC_Salinity();
  ~EC_Salinity();
  float measureEC(float tempCoefficient);
  float measureEC();
  float measureSalinity();
  float measureTemp();
  void  calibrateProbe(float solutionEC,
                       float tempCoef);
  void  calibrateProbeLow(float solutionEC,
                          float tempCoef);
  void  calibrateProbeHigh(float solutionEC,
                           float tempCoef);
  void  setDualPointCalibration(float refLow,
                                float refHigh,
                                float readLow,
                                float readHigh);
  void  setK(float k);
  float getK();
  void  setAccuracy(byte b);
  byte  getAccuracy();
  void  reset();
  void  setTempConstant(byte b);
  byte  getTempConstant();
  void  useTemperatureCompensation(bool b);
  bool  usingTemperatureCompensation();
  void  useDualPoint(bool b);
  bool  usingDualPoint();
  float getCalibrateHigh();
  float getCalibrateLow();
  float getCalibrateHighReading();
  float getCalibrateLowReading();
  void  setCalibrateOffset(float offset);
  float getCalibrateOffset();
  byte  getVersion();

private:

  void  _change_register(byte register);
  void  _send_command(byte command);
  void  _write_register(byte  reg,
                        float f);
  void  _write_byte(byte reg,
                    byte val);
  float _read_register(byte reg);
  byte  _read_byte(byte reg);
};

#endif // ifndef EC_SALINITY_H
