from ecsalinity import ecsalinity

# connect board SDA to pin 19, SCL to pin 23, but any pins can be used.
ec = ecsalinity(19, 23)

ec.measureEC()
print("mS: " + str(ec.mS))
