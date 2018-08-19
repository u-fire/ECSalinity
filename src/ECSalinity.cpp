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

EC_Salinity::EC_Salinity(uint8_t i2c_address)
{
  _address = i2c_address;
  Wire.begin();
}

EC_Salinity::EC_Salinity()
{
  _address = EC_SALINITY;
  Wire.begin();
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

/*!
   \brief Class destructor
 */
EC_Salinity::~EC_Salinity()
{}

/*!
   \code
      float mS = EC_Salinity::measureEC(ec.tempCoefEC, true);
   \endcode

   \brief Starts an EC measurement

   The device starst an EC measurement.
   \param tempCoefficient the coefficient used to compensate for temperature
   \param newTemp boolean to take a new temperature measurement
   \post #uS, #mS, #S, #tempC, #tempF, #PPM_500, #PPM_640, #PPM_700,
 #salinityPPM, #salinityPPT, and #salinityPSU are updated
   \note PPM is not valid if salinity is being measured
   \return milli-Siemens
 */
float EC_Salinity::measureEC(float tempCoefficient, bool newTemp)
{
  if (newTemp)
  {
    measureTemp();
  }

  _write_register(EC_TEMPCOEF_REGISTER, tempCoefficient);
  _send_command(EC_MEASURE_EC);
  delay(EC_EC_MEASUREMENT_TIME);
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
  else
  {
    mS      = -1;
    PPM_500 = -1;
    PPM_640 = -1;
    PPM_700 = -1;
    uS      = -1;
    S       = -1;

    salinityPSU = -1;
    salinityPPT = -1;
    salinityPPM = -1;
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
  return measureEC(tempCoefEC, usingTemperatureCompensation());
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
  measureEC(tempCoefSalinity, usingTemperatureCompensation());
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
        EC_Salinity::setTemp(20.2);
    \endcode

   \brief Sets the temperature used by the device.
   \post #tempC and #tempF are updated
 */
void EC_Salinity::setTemp(float temp_C)
{
  _write_register(EC_TEMP_REGISTER, temp_C);
  tempC = temp_C;
  tempF = ((tempC * 9) / 5) + 32;
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
  delay(EC_EC_MEASUREMENT_TIME + 750);
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
  delay(EC_EC_MEASUREMENT_TIME + 750);
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
  delay(EC_EC_MEASUREMENT_TIME + 750);
  useDualPoint(dualpoint);
}

/*!
    \code
      EC_Salinity::calibrateDry();
    \endcode

   \brief Determines the dry reading of the probe and saves the result in EEPROM.

 */
void EC_Salinity::calibrateDry()
{
  _send_command(EC_DRY);
  delay(EC_EC_MEASUREMENT_TIME + 750);
}

/*!
    \code
      EC_Salinity::getCalibrateDry();
    \endcode

   \brief Gets the dry reading of the probe and saves the result in EEPROM
 */
float EC_Salinity::getCalibrateDry()
{
  return _read_register(EC_DRY_REGISTER);
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
  delay(750);
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
float EC_Salinity::getCalibrateHighReference()
{
  return _read_register(EC_CALIBRATE_REFHIGH_REGISTER);
}

/*!
   \brief Retrieves the dual-point calibration low value
   \return   the dual-point calibration low value
 */
float EC_Salinity::getCalibrateLowReference()
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

/*!
   \brief Configures device to use dual-point calibration
   \param b   true/false
 */
void EC_Salinity::useDualPoint(bool b)
{
  uint8_t retval;
  uint8_t config = _read_byte(EC_CONFIG_REGISTER);

  if (b)
  {
    retval = bitSet(config, EC_DUALPOINT_CONFIG_BIT);
  }
  else
  {
    retval = bitClear(config, EC_DUALPOINT_CONFIG_BIT);
  }

  _write_byte(EC_CONFIG_REGISTER, retval);
}

/*!
   \brief Retrieves the firmware version of the device
   \return   version of firmware
 */
uint8_t EC_Salinity::getVersion()
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
  _write_register(EC_DRY_REGISTER,                NAN);
  setTempConstant(0);
  useDualPoint(false);
  useTemperatureCompensation(false);
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
void EC_Salinity::setTempConstant(uint8_t b)
{
  _write_byte(EC_TEMP_COMPENSATION_REGISTER, b);
}

/*!
   \brief Retrieves the temperature constant
   \return   the temperature to use for compensation
 */
uint8_t EC_Salinity::getTempConstant()
{
  return _read_byte(EC_TEMP_COMPENSATION_REGISTER);
}

/*!
   \code
    EC_Salinity::setI2CAddress(0x3d);
   \endcode
   \brief Changes the i2c address of the device.
   If the default address of the device needs to be changed, call this function to
   permanently change the address. If you forget the i2c address, you will need
   to use an i2c scanner to recover it.
 */
void EC_Salinity::setI2CAddress(uint8_t i2cAddress)
{
  _write_register(EC_SOLUTION_REGISTER, i2cAddress);
  _send_command(EC_I2C);
  _address = i2cAddress;
}

/*!
   \brief Determines if temperature compensation is being used
   \return   true if using compensation, false otherwise
 */
bool EC_Salinity::usingTemperatureCompensation()
{
  uint8_t retval;

  retval = _read_byte(EC_CONFIG_REGISTER);
  return (retval >> 1)  & 0x01;
}

/*!
   \brief Determines if dual point calibration is being used
   \return   true if using compensation, false otherwise
 */
bool EC_Salinity::usingDualPoint()
{
  uint8_t retval;

  retval = _read_byte(EC_CONFIG_REGISTER);
  return (retval >> 0)  & 0x01;
}

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
