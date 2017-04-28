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
#include "RunningMedian.h"

#define EC_SALINITY 0x13                      /*!< EC Salinity probe I2C address */
#define START_MEASUREMENT 0x00                /*!< I2C command to start measurement */
#define SET_K 0x01                            /*!< I2C command to update K */

#define EC_LOW_ACCURACY 3                     /*!< Defines middle of three measurements */
#define EC_HIGH_ACCURACY 9                    /*!< Defines middle of nine measurements */

class EC_Salinity                             /*! EC Salinity Class Implementation */
{
public:
      float S;                                 /*!< EC in Siemens */
      float mS;                                /*!< EC in milli-Siemens */
      long uS;                                /*!< EC in micro-Siemens */
      long PPM_500;                            /*!< Parts per million using 500 as a multiplier */
      long PPM_640;                            /*!< Parts per million using 640 as a multiplier */
      long PPM_700;                            /*!< Parts per million using 700 as a multiplier */
      float salinityPSU;                       /*!< Salinity measured practical salinity units */
      float salinityPPT;                       /*!< Salinity measured parts per thousand */
      long salinityPPM;                        /*!< Salinity measured parts per million */
      float tempC;                             /*!< Temperature in C */
      float tempF;                             /*!< Temperature in F */
      float K;                                 /*!< Cell constant */
      static const float tempCoefEC;           /*!< Temperature compensation coefficient for EC measurement */
      static const float tempCoefSalinity;     /*!< Temperature compensation coefficient for salinity measurement */
      int accuracy = EC_HIGH_ACCURACY;         /*!< Number of samples to take per measurement and must be a multiple of 3*/
      bool useTempCompensation = true;         /*!< To use temperature compensation */
      int tempCompensationConstant = -1;       /*!< To use a constant value for temperature compensation, -1 for actual temperature */
      bool useDualPointCalibration = false;    /*!< Use dual point calibration or not */
      EC_Salinity();
      ~EC_Salinity();
      float measureEC(float tempCoefficient);
      void calibrateProbe(float solutionEC, float tempCoef, int numberProbeSamples);
      void setK(float K);
      void setDualPoint(float referenceLow, float referenceHigh, float readingLow, float readingHigh);
private:
      float _conductivity;
      float _referenceLow;
      float _referenceHigh;
      float _readingLow;
      float _readingHigh;
      void _measureEC();
      void _salinity(float temp);
      union ec_union {
              byte ec[4];
              float ec_val;
              byte t[4];
              float t_val;
              byte k[4];
              float k_val;
      } u_ec;
};

#endif
