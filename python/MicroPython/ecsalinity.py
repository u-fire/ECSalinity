import math
import time

import ustruct
from machine import I2C, Pin

global i2c

EC_SALINITY = 0x3c                  # EC Salinity probe I2C address

EC_MEASURE_EC = 80                  # Command to start an EC measure
EC_MEASURE_TEMP = 40                # Command to measure temperature
EC_CALIBRATE_PROBE = 20             # Command to calibrate the probe
EC_CALIBRATE_LOW = 10               # Command to calibrate the low point of the probe
EC_CALIBRATE_HIGH = 8               # Command to calibrate the high point of the probe
EC_I2C = 1                          # Command to change the i2c address
EC_DRY = 81                         # Command to calibrate the probe for being dry

EC_VERSION_REGISTER = 0             # version register
EC_MS_REGISTER = 1                  # mS register
EC_TEMP_REGISTER = 5                # temperature in C register
EC_K_REGISTER = 9                   # cell constant register
EC_SOLUTION_REGISTER = 13           # calibration solution register
EC_TEMPCOEF_REGISTER = 17           # temperatue coefficient register
EC_CALIBRATE_REFHIGH_REGISTER = 21  # reference low register
EC_CALIBRATE_REFLOW_REGISTER = 25   # reference high register
EC_CALIBRATE_READHIGH_REGISTER = 29  # reading low register
EC_CALIBRATE_READLOW_REGISTER = 33  # reading high register
EC_CALIBRATE_OFFSET_REGISTER = 37   # caliration offset
EC_SALINITY_PSU = 41                # Salinity register
EC_DRY_REGISTER = 45                # Dry calibration register
EC_TEMP_COMPENSATION_REGISTER = 49  # temperature compensation register
EC_CONFIG_REGISTER = 50             # config register
EC_TASK_REGISTER = 51               # task register

EC_EC_MEASUREMENT_TIME = 250          # delay between EC measurements
EC_TEMP_MEASURE_TIME = 750          # delay for temperature measurement

EC_DUALPOINT_CONFIG_BIT = 0         # dual point config bit
EC_TEMP_COMPENSATION_CONFIG_BIT = 1  # temperature compensation config bit

PSU_TO_PPT_CONVERSION = 1.004715    # conversion factor for PSU to PPT


class ecsalinity(object):
    S = 0
    mS = 0
    uS = 0
    PPM_500 = 0
    PPM_640 = 0
    PPM_700 = 0
    salinityPSU = 0
    salinityPPT = 0
    salinityPPM = 0
    tempC = 0
    tempF = 0
    tempCoefEC = 0.019
    tempCoefSalinity = 0.021
    address = EC_SALINITY

    def __init__(self, i2c_bus, scl, sda, **kwargs):
        global i2c
        i2c = I2C(i2c_bus, Pin(scl), Pin(sda))

    def measureTemp(self):
        self._send_command(EC_MEASURE_TEMP)
        time.sleep(EC_TEMP_MEASURE_TIME / 1000.0)
        self.tempC = self._read_register(EC_TEMP_REGISTER)
        self.tempF = ((self.tempC * 9) / 5) + 32
        return self.tempC

    def setTemp(self, temp_C):
        self._write_register(EC_TEMP_REGISTER, temp_C)
        self.tempF = ((self.tempC * 9) / 5) + 32
        
    def measureEC(self, tempCoefficient=None, newTemp=None):
        if tempCoefficient is None:
            tempCoefficient = self.tempCoefEC

        if newTemp is True:
            self.measureTemp()

        if self.usingTemperatureCompensation() is True:
            self.measureTemp()

        self._write_register(EC_TEMPCOEF_REGISTER, tempCoefficient)
        self._send_command(EC_MEASURE_EC)
        time.sleep(EC_EC_MEASUREMENT_TIME / 1000.0)
        self.mS = self._read_register(EC_MS_REGISTER)

        if math.isinf(self.mS) is not True:
            self.PPM_500 = self.mS * 500
            self.PPM_640 = self.mS * 640
            self.PPM_700 = self.mS * 700
            self.uS = self.mS * 1000
            self.S = self.mS / 1000
        else:
            self.mS = -1
            self.PPM_500 = -1
            self.PPM_640 = -1
            self.PPM_700 = -1
            self.uS = -1
            self.S = -1

        self.salinityPSU = self._read_register(EC_SALINITY_PSU)
        self.salinityPPT = self.salinityPSU * PSU_TO_PPT_CONVERSION
        self.salinityPPM = self.salinityPPT * 1000
        return self.mS

    def measureSalinity(self):
        self.measureEC(self.tempCoefSalinity, self.usingTemperatureCompensation())
        return self.salinityPSU

    def calibrateProbe(self, solutionEC, tempCoef):
        dualpoint = self.usingDualPoint()

        self.useDualPoint(0)
        self._write_register(EC_TEMPCOEF_REGISTER, tempCoef)
        self._write_register(EC_SOLUTION_REGISTER, solutionEC)
        self._send_command(EC_CALIBRATE_PROBE)
        time.sleep(EC_EC_MEASUREMENT_TIME / 1000.0)
        self.useDualPoint(dualpoint)

    def calibrateProbeLow(self, solutionEC, tempCoef):
        dualpoint = self.usingDualPoint()

        self.useDualPoint(0)
        self._write_register(EC_TEMPCOEF_REGISTER, tempCoef)
        self._write_register(EC_SOLUTION_REGISTER, solutionEC)
        self._send_command(EC_CALIBRATE_LOW)
        time.sleep(EC_EC_MEASUREMENT_TIME / 1000.0)
        self.useDualPoint(dualpoint)

    def calibrateProbeHigh(self, solutionEC, tempCoef):
        dualpoint = self.usingDualPoint()

        self.useDualPoint(0)
        self._write_register(EC_TEMPCOEF_REGISTER, tempCoef)
        self._write_register(EC_SOLUTION_REGISTER, solutionEC)
        self._send_command(EC_CALIBRATE_HIGH)
        time.sleep(EC_EC_MEASUREMENT_TIME / 1000.0)
        self.useDualPoint(dualpoint)

    def calibrateDry(self):
        self._send_command(EC_DRY)
        time.sleep(EC_EC_MEASUREMENT_TIME / 1000.0)

    def setK(self, k):
        self._write_register(EC_K_REGISTER, k)

    def getK(self):
        return self._read_register(EC_K_REGISTER)

    def getVersion(self):
        return self._read_byte(EC_VERSION_REGISTER)

    def getCalibrateOffset(self):
        return self._read_register(EC_CALIBRATE_OFFSET_REGISTER)

    def getCalibrateHighReference(self):
        return self._read_register(EC_CALIBRATE_REFHIGH_REGISTER)

    def getCalibrateLowReference(self):
        return self._read_register(EC_CALIBRATE_REFLOW_REGISTER)

    def getCalibrateHighReading(self):
        return self._read_register(EC_CALIBRATE_READHIGH_REGISTER)

    def getCalibrateLowReading(self):
        return self._read_register(EC_CALIBRATE_READLOW_REGISTER)

    def getCalibrateDry(self):
        return self._read_register(EC_DRY_REGISTER)

    def reset(self):
        n = float('nan')
        self._write_register(EC_K_REGISTER,                  n)
        self._write_register(EC_CALIBRATE_OFFSET_REGISTER,   n)
        self._write_register(EC_CALIBRATE_REFHIGH_REGISTER,  n)
        self._write_register(EC_CALIBRATE_REFLOW_REGISTER,   n)
        self._write_register(EC_CALIBRATE_READHIGH_REGISTER, n)
        self._write_register(EC_CALIBRATE_READLOW_REGISTER,  n)
        self._write_register(EC_DRY_REGISTER,  n)
        self.setTempConstant(0)
        self.useDualPoint(False)
        self.useTemperatureCompensation(False)

    def setCalibrateOffset(self, offset):
        self._write_register(EC_CALIBRATE_OFFSET_REGISTER, offset)

    def setDualPointCalibration(self, refLow, refHigh, readLow, readHigh):
        self._write_register(EC_CALIBRATE_REFLOW_REGISTER,   refLow)
        self._write_register(EC_CALIBRATE_REFHIGH_REGISTER,  refHigh)
        self._write_register(EC_CALIBRATE_READLOW_REGISTER,  readLow)
        self._write_register(EC_CALIBRATE_READHIGH_REGISTER, readHigh)

    def setTempConstant(self, b):
        self._write_byte(EC_TEMP_COMPENSATION_REGISTER, b)

    def getTempConstant(self):
        return self._read_byte(EC_TEMP_COMPENSATION_REGISTER)

    def setI2CAddress(self, i2cAddress):
        self._write_register(EC_SOLUTION_REGISTER, int(i2cAddress))
        self._send_command(EC_I2C)
        self.address = int(i2cAddress)

    def useTemperatureCompensation(self, b):
        retval = self._read_byte(EC_CONFIG_REGISTER)

        retval = self._bit_set(retval, EC_TEMP_COMPENSATION_CONFIG_BIT, b)
        self._write_byte(EC_CONFIG_REGISTER, retval)

    def useDualPoint(self, b):
        retval = self._read_byte(EC_CONFIG_REGISTER)

        retval = self._bit_set(retval, EC_DUALPOINT_CONFIG_BIT, b)
        self._write_byte(EC_CONFIG_REGISTER, retval)

    def usingTemperatureCompensation(self):
        retval = self._read_byte(EC_CONFIG_REGISTER)
        return (retval >> 1) & 0x01

    def usingDualPoint(self):
        retval = self._read_byte(EC_CONFIG_REGISTER)
        return (retval >> 0) & 0x01

    def _bit_set(self, v, index, x):
        mask = 1 << index
        v &= ~mask
        if x:
            v |= mask
        return v

    def _change_register(self, r):
        global i2c
        i2c.write_byte(self.address, r)
        time.sleep(10 / 1000.0)

    def _send_command(self, command):
        global i2c
        i2c.writeto_mem(self.address, EC_TASK_REGISTER, bytearray([command]))
        time.sleep(10 / 1000.0)

    def _write_register(self, reg, f):
        global i2c
        n = self.round_total_digits(f)
        fd = bytearray(ustruct.pack("f", n))
        i2c.writeto_mem(self.address, reg, fd)
        time.sleep(10 / 1000.0)

    def _read_register(self, reg):
        global i2c
        data = [0, 0, 0, 0]
        data[0] = int.from_bytes(i2c.readfrom_mem(self.address, reg, 1), 'big')
        data[1] = int.from_bytes(i2c.readfrom_mem(
            self.address, reg + 1, 1), 'big')
        data[2] = int.from_bytes(i2c.readfrom_mem(
            self.address, reg + 2, 1), 'big')
        data[3] = int.from_bytes(i2c.readfrom_mem(
            self.address, reg + 3, 1), 'big')
        ba = bytearray(data)
        f = ustruct.unpack('f', ba)[0]
        return self.round_total_digits(f)

    def _write_byte(self, reg, val):
        global i2c
        i2c.writeto_mem(self.address, reg, bytearray([val]))
        time.sleep(10 / 1000.0)

    def _read_byte(self, reg):
        global i2c
        return int.from_bytes(i2c.readfrom_mem(self.address, reg, 1), 'big')

    def magnitude(self, x):
        if math.isnan(x):
            return 0
        if math.isinf(x):
            return 0
        return 0 if x == 0 else int(math.floor(math.log(abs(x), 10))) + 1

    def round_total_digits(self, x, digits=7):
        return round(x, digits - self.magnitude(x))
