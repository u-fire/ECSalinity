import math
import struct
import time
import smbus

global i2c

EC_SALINITY = 0x3c                  # EC Salinity probe I2C address

EC_MEASURE_EC = 80
EC_MEASURE_SW = 40
EC_MEASURE_TEMP = 20
EC_CALIBRATE_EC = 10
EC_CALIBRATE_SW = 8
EC_I2C = 4
EC_READ = 2
EC_WRITE = 1

EC_VERSION_REGISTER = 0             # version register */
EC_FW_VERSION_REGISTER = 1          # firmware version register */
EC_MS_REGISTER = 2                  # mS register */
EC_SALINITY_PSU = 6                 # salinity register */
EC_TEMP_REGISTER = 10               # temperature in C register */
EC_RAW_REGISTER = 14                # raw count register */
EC_SOLUTION_REGISTER = 18           # calibration solution register */
EC_CALIBRATE_EC_REGISTER = 22       # temperatue coefficient register */
EC_CALIBRATE_SW_REGISTER = 26       # reference low register */
EC_TEMP_COMPENSATION_REGISTER = 30  # temperature compensation register */
EC_BUFFER_REGISTER = 34             # buffer register */
EC_CONFIG_REGISTER = 38             # config register */
EC_TASK_REGISTER = 39               # task register */

EC_EC_MEASUREMENT_TIME = 250        # delay between EC measurements
EC_TEMP_MEASURE_TIME = 750          # delay for temperature measurement

EC_TEMP_COMPENSATION_CONFIG_BIT = 0  # temperature compensation config bit


class ecsalinity(object):
    S = 0
    mS = 0
    uS = 0
    raw = 0
    PPM_500 = 0
    PPM_640 = 0
    PPM_700 = 0
    salinityPSU = 0
    tempC = 0
    tempF = 0
    address = EC_SALINITY

    def __init__(self, address=EC_SALINITY, i2c_bus=3, **kwargs):
        global i2c
        self.address = address
        i2c = smbus.SMBus(i2c_bus)
# measurements

    def _measure(self, EC, newTemp=None):
        if newTemp is True:
            self.measureTemp()

        if EC is True:
            self._send_command(EC_MEASURE_EC)
        else:
            self._send_command(EC_MEASURE_SW)

        time.sleep(EC_EC_MEASUREMENT_TIME / 1000.0)
        self.mS = self._read_register(EC_MS_REGISTER)
        self.raw = self._read_register(EC_RAW_REGISTER)

        if self.raw == 0:
            self.mS = float('inf')

        if math.isinf(self.mS) is not True:
            self.PPM_500 = self.mS * 500
            self.PPM_640 = self.mS * 640
            self.PPM_700 = self.mS * 700
            self.uS = self.mS * 1000
            self.S = self.mS / 1000

            self.salinityPSU = self._read_register(EC_SALINITY_PSU)
        else:
            self.mS = -1
            self.PPM_500 = -1
            self.PPM_640 = -1
            self.PPM_700 = -1
            self.uS = -1
            self.S = -1
            self.salinityPSU = -1

        return self.mS

    def measureEC(self, newTemp=None):
        if newTemp is None:
            return self._measure(True, self.usingTemperatureCompensation())
        else:
            return self._measure(True, newTemp)

    def measureSW(self, newTemp=None):
        if newTemp is None:
            return self._measure(False, self.usingTemperatureCompensation())
        else:
            return self._measure(False, newTemp)

    def measureTemp(self):
        self._send_command(EC_MEASURE_TEMP)
        time.sleep(EC_TEMP_MEASURE_TIME / 1000.0)
        self.tempC = self._read_register(EC_TEMP_REGISTER)

        if self.tempC is -127.0:
            self.tempF = -127.0
        else:
            self.tempF = ((self.tempC * 9) / 5) + 32

        return self.tempC

# calibration
    def calibrateEC(self, solutionEC):
        self._write_register(EC_SOLUTION_REGISTER, solutionEC)
        self._send_command(EC_CALIBRATE_EC)
        time.sleep(EC_TEMP_MEASURE_TIME / 1000.0)

    def getCalibrationEC(self):
        return self._read_register(EC_CALIBRATE_EC_REGISTER)

    def calibrateSW(self, solutionSW):
        self._write_register(EC_SOLUTION_REGISTER, solutionSW)
        self._send_command(EC_CALIBRATE_SW)
        time.sleep(EC_TEMP_MEASURE_TIME / 1000.0)

    def getCalibrationSW(self):
        return self._read_register(EC_CALIBRATE_SW_REGISTER)

# temperature
    def setTemp(self, temp_C):
        self._write_register(EC_TEMP_REGISTER, temp_C)
        self.tempC = temp_C
        self.tempF = ((self.tempC * 9) / 5) + 32

    def setTempConstant(self, b):
        self._write_byte(EC_TEMP_COMPENSATION_REGISTER, b)

    def getTempConstant(self):
        return self._read_byte(EC_TEMP_COMPENSATION_REGISTER)

    def useTemperatureCompensation(self, b):
        retval = self._read_byte(EC_CONFIG_REGISTER)

        retval = self._bit_set(retval, EC_TEMP_COMPENSATION_CONFIG_BIT, b)
        self._write_byte(EC_CONFIG_REGISTER, retval)

    def usingTemperatureCompensation(self):
        retval = self._read_byte(EC_CONFIG_REGISTER)
        return (retval >> EC_TEMP_COMPENSATION_CONFIG_BIT) & 0x01

# utilities
    def getVersion(self):
        return self._read_byte(EC_VERSION_REGISTER)

    def getFirmware(self):
        return self._read_byte(EC_FW_VERSION_REGISTER)

    def reset(self):
        n = float('nan')
        self._write_register(EC_CALIBRATE_EC_REGISTER, n)
        self._write_register(EC_CALIBRATE_SW_REGISTER, n)
        self.setTempConstant(25)
        self.useTemperatureCompensation(False)

    def setI2CAddress(self, i2cAddress):
        self._write_register(EC_BUFFER_REGISTER, float(i2cAddress))
        self._send_command(EC_I2C)
        self.address = int(i2cAddress)

    def connected(self):
        retval = self._read_byte(EC_VERSION_REGISTER)

        if retval != 0xFF:
            return True
        else:
            return False

    def readEEPROM(self, address):
        self._write_register(EC_SOLUTION_REGISTER, address)
        self._send_command(EC_READ)
        return self._read_register(EC_BUFFER_REGISTER)

    def writeEEPROM(self, address, value):
        self._write_register(EC_SOLUTION_REGISTER, address)
        self._write_register(EC_BUFFER_REGISTER, value)
        self._send_command(EC_WRITE)

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
        i2c.write_byte_data(self.address, EC_TASK_REGISTER, command)
        time.sleep(10 / 1000.0)

    def _write_register(self, reg, f):
        global i2c
        n = self.round_total_digits(f)
        fd = bytearray(struct.pack("f", n))
        data = [0, 0, 0, 0]
        data[0] = fd[0]
        data[1] = fd[1]
        data[2] = fd[2]
        data[3] = fd[3]
        self._change_register(reg)
        i2c.write_i2c_block_data(self.address, reg, data)
        time.sleep(10 / 1000.0)

    def _read_register(self, reg):
        global i2c
        data = [0, 0, 0, 0]
        self._change_register(reg)
        data[0] = i2c.read_byte(self.address)
        data[1] = i2c.read_byte(self.address)
        data[2] = i2c.read_byte(self.address)
        data[3] = i2c.read_byte(self.address)
        ba = bytearray(data)
        f = struct.unpack('f', ba)[0]
        return self.round_total_digits(f)

    def _write_byte(self, reg, val):
        global i2c
        i2c.write_byte_data(self.address, reg, val)
        time.sleep(10 / 1000.0)

    def _read_byte(self, reg):
        global i2c
        self._change_register(reg)
        time.sleep(10 / 1000.0)
        return i2c.read_byte(self.address)

    def magnitude(self, x):
        if math.isnan(x):
            return 0
        return 0 if x == 0 else int(math.floor(math.log10(abs(x)))) + 1

    def round_total_digits(self, x, digits=7):
        return round(x, digits - self.magnitude(x))
