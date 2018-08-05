from ecsalinity import ecsalinity

# the passed parameter 3 indicates the third (/dev/i2c-3) I2C device
ec = ecsalinity(0x3c, 3)

ec.measureEC()
print("mS: " + str(ec.mS))
