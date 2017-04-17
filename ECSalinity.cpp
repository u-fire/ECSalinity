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
   \file ec.cpp
   \brief EC_Salinity Class Implementation
 */

#include "ECSalinity.h"

const float EC_Salinity::tempCoefEC = 0.019;
const float EC_Salinity::tempCoefSalinity = 0.021;

/*!
   \brief Class constructor
 */
EC_Salinity::EC_Salinity()
{
        Wire.begin();
}

/*!
   \brief Class destructor
 */
EC_Salinity::~EC_Salinity()
{
}

/*!
   \brief Starts an EC/Salinity measurement

   #sampleNumber of samples are taken and the average of the middle third of
   the measures is returned in #mS. #tempC is treated the same way.
   \param tempCoefficient the temperature coefficient used to compensate
   for temperature.
   \pre #K must be set
   \post #uS, #mS, #dS, #S, #tempC, #tempF, PPMs, and #K are updated
   \note PPM is not valid if salinity is being measured
   \return #mS of measurement
 */
float EC_Salinity::measureEC(float tempCoefficient)
{
        RunningMedian conductivityMedian = RunningMedian(sampleNumber);
        RunningMedian tempCMedian = RunningMedian(sampleNumber);
        int middleThird = sampleNumber / 3;

        int i = sampleNumber;
        while(i--)
        {
                _measureEC();
                conductivityMedian.add(_conductivity);
                tempCMedian.add(tempC);
        }

        mS = 1000 / (conductivityMedian.getAverage(middleThird) * K);
        tempF = ((tempCMedian.getAverage(middleThird) * 9) / 5) + 32;

        mS  =  mS / (1 + tempCoefficient * (tempCMedian.getAverage(middleThird) - 25.0));
        uS = mS * 1000;
        dS = mS / 1000;
        S = dS / 1000;

        if (mS > 0.01)
        {
                PPM_500 = mS * 500;
                PPM_640 = mS * 640;
                PPM_700 = mS * 700;
        }
        else
        {
                PPM_500 = 0;
                PPM_640 = 0;
                PPM_700 = 0;
        }

        _Salinity(tempCMedian.getAverage(middleThird));

        return mS;
}

/*!
   \brief Calibrates the connected probe and saves the result in EEPROM
   numberProbeSamples measurements are made, the median third is taken and
   averaged together to get a #K value for the probe.
   \param solutionEC          the EC of the calibration solution
   \param tempCoef            the coefficient used to calibrate the probe
   \param numberProbeSamples  the number of samples to take to calibrate the probe (must be a multiple of 3)
   \post  #K                  will be saved in EEPROM and used automatically thereafter
 */
void EC_Salinity::calibrateProbe(float solutionEC, float tempCoef, int numberProbeSamples)
{
        float ec;
        RunningMedian conductivityMedian = RunningMedian(numberProbeSamples);
        RunningMedian tempCMedian = RunningMedian(numberProbeSamples);
        int middleThird = numberProbeSamples / 3;

        int i = numberProbeSamples;
        while(i--)
        {
                _measureEC();
                conductivityMedian.add(_conductivity);
                tempCMedian.add(tempC);
                delay(250);
        }

        ec  =  solutionEC * (1 + tempCoef * (tempCMedian.getAverage(middleThird) - 25.0));

        K = 1000 / ( conductivityMedian.getAverage(middleThird) * ec );
        setK(K);
}

/*!
   \brief Updates the slave device with a new cell constant and saved in EEPROM
   \param K   the new cell constant
 */
void EC_Salinity::setK(float K)
{
        u_ec.k_val = K;

        Wire.beginTransmission(EC_SALINITY);

        Wire.write(SET_K);
        Wire.write(u_ec.k[0]);
        Wire.write(u_ec.k[1]);
        Wire.write(u_ec.k[2]);
        Wire.write(u_ec.k[3]);

        Wire.endTransmission();
}

/*!
   \brief Private member to measure salinity
   \param temp the temperature of the solution being measured
   \post #salinityPSU and #salinityPPT will be updated with the new value or
   -1 if the value is out of range.
 */
void EC_Salinity::_Salinity(float temp)
{
        float r, ds, r2;

        float a0 = 0.008;
        float a1 = -0.1692;
        float a2 = 25.3851;
        float a3 = 14.0941;
        float a4 = -7.0261;
        float a5 = 2.7081;

        float b0 = 0.0005;
        float b1 = -0.0056;
        float b2 = -0.0066;
        float b3 = -0.0375;
        float b4 = 0.0636;
        float b5 = -0.0144;

        float c0 = 0.6766097;
        float c1 = 0.0200564;
        float c2 = 0.0001104259;
        float c3 = -0.00000069698;
        float c4 = 0.0000000010031;

        r = uS / 42900;

        r /= (c0 + temp * (c1 + temp * (c2 + temp * (c3 + temp * c4))));

        r2 = sqrtf(r);
        ds = b0 + r2 * (b1 + r2 * (b2 + r2 * (b3 + r2 * (b4 + r2 * b5))));
        ds = ds * ((temp - 15.0) / (1.0 + 0.0162 * (temp - 15.0)));

        // the formula to convert between EC and PSU is not valid for values
        // below 2 and above 42, or for temperatures below -2C or above 35C.
        salinityPPT = -1;
        salinityPPM = -1;
        salinityPSU = a0 + r2 * (a1 + r2 * (a2 + r2 * (a3 + r2 * (a4 + r2 * a5)))) + ds;

        if (salinityPSU < 2 || salinityPSU > 42)
        {
          salinityPSU = -1;
          return;
        }

        if (tempC < -2 || tempC > 35)
        {
          salinityPSU = -1;
          return;
        }

        salinityPPT = salinityPSU * 1.004715;
        salinityPPM = salinityPPT * 1000;
}

/*!
   \brief Private member to address the device and start a measurment.
   \post #_conductivity, #tempC, and #K will be updated with the new value
 */
void EC_Salinity::_measureEC()
{
        int i = 0;
        byte b[12];

        Wire.beginTransmission(EC_SALINITY);
        Wire.write(START_MEASUREMENT);
        Wire.endTransmission();
        delay(800);

        Wire.requestFrom(EC_SALINITY, 12);
        while (Wire.available())
        {
                b[i] = Wire.read();
                i++;
        }

        u_ec.ec[0] = b[0];
        u_ec.ec[1] = b[1];
        u_ec.ec[2] = b[2];
        u_ec.ec[3] = b[3];
        _conductivity = u_ec.ec_val;

        u_ec.t[0] = b[4];
        u_ec.t[1] = b[5];
        u_ec.t[2] = b[6];
        u_ec.t[3] = b[7];
        tempC = u_ec.t_val;

        u_ec.k[0] = b[8];
        u_ec.k[1] = b[9];
        u_ec.k[2] = b[10];
        u_ec.k[3] = b[11];
        K = u_ec.k_val;
}
