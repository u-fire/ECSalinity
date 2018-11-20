import cmd
from ecsalinity import ecsalinity

# connect board SCL to pin 5, SDA to pin 4, but any pins can be used. -1 indicates software I2C.
ec = ecsalinity(19, 23, 61)


class ECShell(cmd.Cmd):
    prompt = '> '

    def do_config(self, a):
        """prints out all the configuration data\nparameters: none"""
        print("EC Interface Config: " +
              'connected' if ec.connected() else '**disconnected**')
        print("\tcalibration: ")
        print("\t  EC: " + str(ec.getCalibrationEC()))
        print("\t  SW: " + str(ec.getCalibrationSW()))
        print("\ttemp. compensation: " + str(ec.usingTemperatureCompensation()))
        print("\t\tconstant: " + str(ec.getTempConstant()))
        print("\tversion: " + (str(ec.getVersion())) +
              "." + (str(ec.getFirmware())))

    def do_reset(self, a):
        """reset all saved values\nparameters: none"""
        ec.reset()

    def do_temp(self, temp_C):
        """measures the temperature\nparameters: none"""
        if temp_C:
            ec.setTemp(float(temp_C))
        else:
            ec.measureTemp()

        ec.measureTemp()
        print("C/F: " + str(ec.tempC) + " / " + str(ec.tempF))

    def do_raw(self, a):
        """starts an EC measurement\nparameters: none"""
        ec.measureEC()
        print("raw: " + str(ec.raw))

    def do_ec(self, a):
        """starts an EC measurement\nparameters: none"""
        ec.measureEC()
        print("mS: " + str(ec.mS))

    def do_tc(self, arg):
        """returns or sets temperature compensation information\nparameters\n\tbool to use compensation\n\ttemperature constant to use (255 for actual)"""
        a = arg.split()

        if len(a) >= 1:
            ec.useTemperatureCompensation(int(a[0]))

        if len(a) >= 2:
            ec.setTempConstant(int(a[1]))

        print("\ttemp. compensation: " + str(ec.usingTemperatureCompensation()))
        print("\t\tconstant: " + str(ec.getTempConstant()))

    def do_sw(self, a):
        """measures salinity\nparameters: none"""
        ec.measureSW()
        print("salinity PSU: " + str(ec.salinityPSU))

    def do_data(self, a):
        """prints all the data registers"""
        print("raw: " + str(ec.raw))
        print("S: " + str(ec.S))
        print("mS: " + str(ec.mS))
        print("uS: " + str(ec.uS))
        print("TDS 500 | 640 | 700: " + str(ec.PPM_500) +
              " | " + str(ec.PPM_640) + " | " + str(ec.PPM_700))
        print("salinity PSU: " + str(ec.salinityPSU))

    def do_version(self, a):
        """prints the version register"""
        print("\tversion: " + (str(ec.getVersion())) +
              "." + (str(ec.getFirmware())))

    def do_i2c(self, i2cAddress):
        """changes the I2C address"""
        ec.setI2CAddress(i2cAddress)

    def do_read(self, address):
        print(ec.readEEPROM(address))

    def do_write(self, arg):
        a = arg.split()
        ec.writeEEPROM(a[0], a[1])

    def do_EOF(self, line):
        return True


ECShell().cmdloop()
