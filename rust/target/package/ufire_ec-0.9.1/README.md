### EC-Salinity Probe Interface

>Monitor hydroponic nutrient levels or salinity in aquariums, pools, and soil.
* Electrical Conductivity in Siemens
* Total Dissolved Solids in PPM
* Salinity in PSU and PPT
* Temperature in Celsius/Fahrenheit

#### What it is
An I2C sensor device, an optional DS18B20 waterproof temperature sensor, and a two-electrode EC probe. It measures the conductivity of a solution and converts it into Siemens (S). From that value, it derives total dissolved solids and salinity. The firmware on the device provides two [calibration options](https://ufire.co/ECSalinity/#calibration), single or dual point which can be used simultaneously. Temperature compensation is also provided in the firmware.

Multiple EC-Salinity probes can be connected to the same master device and in the same solution without causing interference. The [ISE Probe Interface](/ise/) can be also used with minimal interference.

#### Using it
There is extensive [documentation](https://ufire.co/ECSalinity/) on the [specifications](https://ufire.co/ECSalinity/#characteristics), [setup](https://ufire.co/ECSalinity/#getting-started), and [use](https://ufire.co/ECSalinity/#use) of the device. The library to use it is in the Arduino and Particle.io IDE; a python implementation for Raspberry Pi and MicroPython is also available.
