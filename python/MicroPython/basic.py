from ecsalinity import ecsalinity

# connect board SCL to pin 5, SDA to pin 4, but any pins can be used. -1 indicates software I2C.
ec = ecsalinity(-1, 5, 4)

ec.calculateK(2.77, 0.019)

print("config: ")
print("\tK: " + str(ec.getK()))
print("\toffset: " + str(ec.getCalibrateOffset()))
print("\tdry: " + str(ec.getCalibrateDry()))
print("\tdual point: " + str(ec.usingDualPoint()))
print("\tlow reference / read: " + str(ec.getCalibrateLowReference()) +
      " / " + str(ec.getCalibrateLowReading()))
print("\thigh reference / reading: " + str(ec.getCalibrateHighReference()) +
      " / " + str(ec.getCalibrateHighReading()))
print("\ttemp. compensation: " + str(ec.usingTemperatureCompensation()))
print("\t\tconstant: " + str(ec.getTempConstant()))
print("\tversion: " + (hex(ec.getVersion())))

ec.measureEC()
print("mS: " + str(ec.mS))
print("C/F: " + str(ec.tempC) + " / " + str(ec.tempF))
