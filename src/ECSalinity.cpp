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
   \file ECSalinity.cpp
   \brief EC_Salinity Class Implementation
 */

#include "ECSalinity.h"

const float EC_Salinity::tempCoefEC       = 0.019;
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
   \code
      float mS = EC_Salinity::measureEC(ec.tempCoefEC);
   \endcode

   \brief Starts an EC measurement

   The device starst an EC measurement. The accuracy can be specified in
 #EC_Salinity::setAccuracy.
   \param tempCoefficient the coefficient used to compensate for temperature.
   \post #uS, #mS, #S, #tempC, #tempF, #PPM_500, #PPM_640, #PPM_700,
 #salinityPPM, #salinityPPT, and #salinityPSU are updated
   \note PPM is not valid if salinity is being measured
   \return milli-Siemens
 */
float EC_Salinity::measureEC(float tempCoefficient)
{
        _write_register(EC_TEMPCOEF_REGISTER, tempCoefficient);
        _send_command(EC_MEASURE_EC);
        delay((getAccuracy() * EC_EC_MEASURMENT_TIME));
        mS = _read_register(EC_MS_REGISTER);

        if (mS == mS)
        {
                PPM_500 = mS * 500;
                PPM_640 = mS * 640;
                PPM_700 = mS * 700;
                uS      = mS * 1000;
                S       = mS / 1000;

                salinityPSU = _read_register(EC_SALINITY_PSU);
                salinityPPT = salinityPSU * PSU_TO_PPT_CONVERSION;
                salinityPPM = salinityPPT * 1000;
        }

        return mS;
}

/*!
    \code
      float mS = EC_Salinity::measureEC();
    \endcode

   \brief Convenience function to measure EC in freshwater.

   Calls #EC_Salinity::measureEC(#EC_Salinity::tempCoefEC)
   \return EC in mS
 */
float EC_Salinity::measureEC()
{
        return measureEC(tempCoefEC);
}

/*!
   \code
       float PSU = EC_Salinity::measureSalinity();
   \endcode

   \brief Convenience function to measure salinity.

   Calls #EC_Salinity::measureEC(#EC_Salinity::tempCoefSalinity)
   \return salinity in PSU
 */
float EC_Salinity::measureSalinity()
{
        measureEC(tempCoefSalinity);
        return salinityPSU;
}

/*!
    \code
        float tempC = EC_Salinity::measureTemp();
    \endcode

   \brief Starts a temperature measurement
   \post #tempC and #tempF are updated
   \note A value of -127 means the device is not connected.
   \return temperature in C
 */
float EC_Salinity::measureTemp()
{
        _send_command(EC_MEASURE_TEMP);
        delay(EC_TEMP_MEASURE_TIME);
        tempC = _read_register(EC_TEMP_REGISTER);
        tempF = ((tempC * 9) / 5) + 32;
        return tempC;
}

/*!
    \code
      EC_Salinity::calibrateProbe(2.77, EC_Salinity::tempCoefEC);
    \endcode

   \brief Calibrates the connected probe and saves the result in EEPROM
   \param solutionEC          the EC of the calibration solution in mS
   \param tempCoef            the coefficient used to calibrate the probe
   \post                      offset will be saved in the device's EEPROM
                              and used automatically thereafter
 */
void EC_Salinity::calibrateProbe(float solutionEC, float tempCoef)
{
        bool dualpoint = usingDualPoint();

        useDualPoint(false);
        _write_register(EC_TEMPCOEF_REGISTER, tempCoef);
        _write_register(EC_SOLUTION_REGISTER, solutionEC);
        _send_command(EC_CALIBRATE_PROBE);
        delay((getAccuracy() * EC_EC_MEASURMENT_TIME));
        useDualPoint(dualpoint);
}

/*!
    \code
      EC_Salinity::calibrateProbeLow(1.0, EC_Salinity::tempCoefEC);
    \endcode

   \brief Calibrates the dual-point values for the low reading and saves them
   in the devices's EEPROM.
   \param solutionEC          the EC of the calibration solution in mS
   \param tempCoef            the coefficient used to calibrate the probe
 */
void EC_Salinity::calibrateProbeLow(float solutionEC, float tempCoef)
{
        bool dualpoint = usingDualPoint();

        useDualPoint(false);
        _write_register(EC_TEMPCOEF_REGISTER, tempCoef);
        _write_register(EC_SOLUTION_REGISTER, solutionEC);
        _send_command(EC_CALIBRATE_LOW);
        delay((getAccuracy() * EC_EC_MEASURMENT_TIME));
        useDualPoint(dualpoint);
}

/*!
   \code
      EC_Salinity::calibrateProbeHigh(3.0, EC_Salinity::tempCoefEC);
   \endcode


   \brief Calibrates the dual-point values for the high reading and saves them
   in the devices's EEPROM.
   \param solutionEC          the EC of the calibration solution in mS
   \param tempCoef            the coefficient used to calibrate the probe
 */
void EC_Salinity::calibrateProbeHigh(float solutionEC, float tempCoef)
{
        bool dualpoint = usingDualPoint();

        useDualPoint(false);
        _write_register(EC_TEMPCOEF_REGISTER, tempCoef);
        _write_register(EC_SOLUTION_REGISTER, solutionEC);
        _send_command(EC_CALIBRATE_HIGH);
        delay((getAccuracy() * EC_EC_MEASURMENT_TIME));
        useDualPoint(dualpoint);
}

/*!
  \code
    EC_Salinity::calculateK(2.77, EC_Salinity::tempCoefEC);
  \endcode

   \brief Calculates the K value of the connected probe and saves it in EEPROM.
   \param solutionEC          the EC of the calibration solution in mS
   \param tempCoef            the coefficient used to calibrate the probe
 */
void EC_Salinity::calculateK(float solutionEC, float tempCoef)
{
        bool dualpoint = usingDualPoint();

        useDualPoint(false);
        _write_register(EC_TEMPCOEF_REGISTER, tempCoef);
        _write_register(EC_SOLUTION_REGISTER, solutionEC);
        _send_command(EC_CALCULATE_K);
        delay((getAccuracy() * EC_EC_MEASURMENT_TIME));
        useDualPoint(dualpoint);
}

/*!
  \code
    EC_Salinity::setDualPointCalibration(1.0, 3.0, 0.9, 3.2);
  \endcode

   \brief Sets all the values for dual point calibration and saves them
   in the devices's EEPROM.
   \param refLow            the reference low point
   \param refHigh           the reference high point
   \param readLow           the measured low point in mS
   \param readHigh          the measured high point in mS
 */
void EC_Salinity::setDualPointCalibration(float refLow,
                                          float refHigh,
                                          float readLow,
                                          float readHigh)
{
        _write_register(EC_CALIBRATE_REFLOW_REGISTER,   refLow);
        _write_register(EC_CALIBRATE_REFHIGH_REGISTER,  refHigh);
        _write_register(EC_CALIBRATE_READLOW_REGISTER,  readLow);
        _write_register(EC_CALIBRATE_READHIGH_REGISTER, readHigh);
}

/*!
  \code
    EC_Salinity::setK(1.121);
  \endcode

   \brief Updates the device with a new cell constant and saves it in EEPROM
   \param k   the new cell constant
 */
void EC_Salinity::setK(float k)
{
        _write_register(EC_K_REGISTER, k);
}

/*!
  \code
    float k = EC_Salinity::getK();
  \endcode

   \brief Retrieves the cell constant from the device.
   \return   the new cell constant
 */
float EC_Salinity::getK()
{
        return _read_register(EC_K_REGISTER);
}

/*!
  \code
    float calibrateOffset = EC_Salinity::getCalibrateOffset();
  \endcode

   \brief Retrieves the single point offset value
   \return   single point offset value
 */
float EC_Salinity::getCalibrateOffset()
{
        return _read_register(EC_CALIBRATE_OFFSET_REGISTER);
}

/*!
   \brief Retrieves the dual-point calibration high value
   \return   the dual-point calibration high value
 */
float EC_Salinity::getCalibrateHigh()
{
        return _read_register(EC_CALIBRATE_REFHIGH_REGISTER);
}

/*!
   \brief Retrieves the dual-point calibration low value
   \return   the dual-point calibration low value
 */
float EC_Salinity::getCalibrateLow()
{
        return _read_register(EC_CALIBRATE_REFLOW_REGISTER);
}

/*!
   \brief Retrieves the dual-point calibration reading high value
   \return   the dual-point calibration high value
 */
float EC_Salinity::getCalibrateHighReading()
{
        return _read_register(EC_CALIBRATE_READHIGH_REGISTER);
}

/*!
   \brief Retrieves the dual-point calibration reading low value
   \return   the dual-point calibration low value
 */
float EC_Salinity::getCalibrateLowReading()
{
        return _read_register(EC_CALIBRATE_READLOW_REGISTER);
}

/*!
  \code
    EC_Salinity::useTemperatureCompensation(true);
  \endcode

   \brief Configures device to use temperature compensation or not
   \param b   true/false
 */
void EC_Salinity::useTemperatureCompensation(bool b)
{
        byte retval;

        retval = _read_byte(EC_CONFIG_REGISTER);
        if (b)
        {
                retval = bitSet(retval, EC_TEMP_COMPENSATION_CONFIG_BIT);
        }
        else
        {
                retval = bitClear(retval, EC_TEMP_COMPENSATION_CONFIG_BIT);
        }
        _write_byte(EC_CONFIG_REGISTER, retval);
}

/*!
   \brief Configures device to use dual-point calibration
   \param b   true/false
 */
void EC_Salinity::useDualPoint(bool b)
{
        byte retval;

        retval = _read_byte(EC_CONFIG_REGISTER);
        if (b)
        {
                retval = bitSet(retval, EC_DUALPOINT_CONFIG_BIT);
        }
        else
        {
                retval = bitClear(retval, EC_DUALPOINT_CONFIG_BIT);
        }
        _write_byte(EC_CONFIG_REGISTER, retval);
}

/*!
   \brief Retrieves the firmware version of the device
   \return   version of firmware
 */
byte EC_Salinity::getVersion()
{
        return _read_byte(EC_VERSION_REGISTER);
}

/*!
   \brief Resets all the stored calibration information.
 */
void EC_Salinity::reset()
{
        _write_register(EC_K_REGISTER,                  NAN);
        _write_register(EC_CALIBRATE_OFFSET_REGISTER,   NAN);
        _write_register(EC_CALIBRATE_REFHIGH_REGISTER,  NAN);
        _write_register(EC_CALIBRATE_REFLOW_REGISTER,   NAN);
        _write_register(EC_CALIBRATE_READHIGH_REGISTER, NAN);
        _write_register(EC_CALIBRATE_READLOW_REGISTER,  NAN);
}

/*!
  \code
    EC_Salinity::setAccuracy(6);
  \endcode

   \brief Configures the accuracy of the device.

   The device maintains a running median of values. It throws out the top and
   bottom third of values, then averages the middle third together to return
   a single value. The accuracy increases with a high number. It must be
   evenly divisible by 3.
   \param b   accuracy of the device
 */
void EC_Salinity::setAccuracy(byte b)
{
        _write_byte(EC_ACCURACY_REGISTER, b);
}

/*!
   \brief Retrieves the accuracy configuration of the device
   \return   accuracy
 */
byte EC_Salinity::getAccuracy()
{
        return _read_byte(EC_ACCURACY_REGISTER);
}

/*!
   \brief Sets the single point offset value
   \param offset   single point offset value
 */
void EC_Salinity::setCalibrateOffset(float offset)
{
        _write_register(EC_CALIBRATE_OFFSET_REGISTER, offset);
}

/*!
  \code
    EC_Salinity::setTempConstant(25);
    EC_Salinity::setTempConstant(0xFF);   // use the actual tempeature
  \endcode

   \brief Configures device to use the provided temperature constant

   By default, the temperature constant is set to 0xFF which instructs the actual
   temperature to be used for temperature compensation, however any number
   can be specified. To use the actual temperature, restore the value to 0xFF.
   \param b   the temperature to use for compensation
 */
void EC_Salinity::setTempConstant(byte b)
{
        _write_byte(EC_TEMP_COMPENSATION_REGISTER, b);
}

/*!
   \brief Retrieves the temperature constant
   \return   the temperature to used for compensation
 */
byte EC_Salinity::getTempConstant()
{
        return _read_byte(EC_TEMP_COMPENSATION_REGISTER);
}

/*!
   \brief Determines if temperature compensation is being used
   \return   true if using compensation, false otherwise
 */
bool EC_Salinity::usingTemperatureCompensation()
{
        byte retval;

        retval = _read_byte(EC_CONFIG_REGISTER);
        return (retval >> 1)  & 0x01;
}

/*!
   \brief Determines if dual point calibration is being used
   \return   true if using compensation, false otherwise
 */
bool EC_Salinity::usingDualPoint()
{
        byte retval;

        retval = _read_byte(EC_CONFIG_REGISTER);
        return (retval >> 0)  & 0x01;
}

void EC_Salinity::_change_register(byte r)
{
        Wire.beginTransmission(EC_SALINITY);
        Wire.write(r);
        Wire.endTransmission();
        delay(10);
}

void EC_Salinity::_send_command(byte command)
{
        Wire.beginTransmission(EC_SALINITY);
        Wire.write(EC_TASK_REGISTER);
        Wire.write(command);
        Wire.endTransmission();
        delay(10);
}

void EC_Salinity::_write_register(byte reg, float f)
{
        byte b[5];
        float f_val = f;

        b[0] = reg;
        b[1] = *((uint8_t *)&f_val);
        b[2] = *((uint8_t *)&f_val + 1);
        b[3] = *((uint8_t *)&f_val + 2);
        b[4] = *((uint8_t *)&f_val + 3);
        Wire.beginTransmission(EC_SALINITY);
        Wire.write(b, 5);
        Wire.endTransmission();
        delay(10);
}

float EC_Salinity::_read_register(byte reg)
{
        float retval;

        _change_register(reg);
        Wire.requestFrom(EC_SALINITY, 1);
        *((uint8_t *)&retval) = Wire.read();
        Wire.requestFrom(EC_SALINITY, 1);
        *((uint8_t *)&retval + 1) = Wire.read();
        Wire.requestFrom(EC_SALINITY, 1);
        *((uint8_t *)&retval + 2) = Wire.read();
        Wire.requestFrom(EC_SALINITY, 1);
        *((uint8_t *)&retval + 3) = Wire.read();
        delay(10);
        return retval;
}

void EC_Salinity::_write_byte(byte reg, byte val)
{
        byte b[5];

        b[0] = reg;
        b[1] = val;
        Wire.beginTransmission(EC_SALINITY);
        Wire.write(b, 2);
        Wire.endTransmission();
        delay(10);
}

byte EC_Salinity::_read_byte(byte reg)
{
        byte retval;

        _change_register(reg);
        Wire.requestFrom(EC_SALINITY, 1);
        retval = Wire.read();
        delay(10);
        return retval;
}
