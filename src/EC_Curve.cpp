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

#include "EC_Curve.h"

float EC_Curve::measureEC(float tempCoefficient, bool newTemp)
{
  float raw = EC_Salinity::measureEC(tempCoefEC, usingTemperatureCompensation());

  // polynomial least squares
  // mS = (2.972381284 * 0.0000001 * (raw * raw)) - (4.038016888 * 0.001 * raw) + 14.29572727;

  // power least squares
  mS = 112800810 * pow(raw, -2.122233479);

  if (usingTemperatureCompensation()) {
    mS =  mS / (1.0 + tempCoefficient * (tempC - getTempConstant()));
  }

  if (mS == mS)
  {
    PPM_500 = mS * 500;
    PPM_640 = mS * 640;
    PPM_700 = mS * 700;
    uS      = mS * 1000;
    S       = mS / 1000;
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
    raw         = -1;
  }

  return mS;
}

float EC_Curve::measureEC()
{
  return measureEC(tempCoefEC, usingTemperatureCompensation());
}

float EC_Curve::measureSalinity()
{
  return measureEC(tempCoefSalinity, usingTemperatureCompensation());
}
