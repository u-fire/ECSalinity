### EC Salinity Probe Interface for Raspberry Pi

Read the [documentation](http://ufire.co/ECSalinity/#getting-started) regarding adding an I2C overlay to use software I2C, since the Raspberry's implementation has a bug.

#### Running the Examples
1. git clone https://github.com/u-fire/ECSalinity.git --depth=1
2. cd ECSalinity/python/RaspberryPi
3. sudo python3 basic.py
4. sudo python3 shell.py

~~~
from ecsalinity import ecsalinity
ec = ecsalinity(0x3c, 3)

print("mS: " + str(ec.measureEC()))
~~~
