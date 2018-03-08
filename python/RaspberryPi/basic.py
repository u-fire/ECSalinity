from ecsalinity import ecsalinity

ec = ecsalinity(3)

ec.measureEC()
print("mS: " + str(ec.mS))
