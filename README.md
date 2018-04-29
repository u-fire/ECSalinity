[![GitHub release](https://img.shields.io/github/release/u-fire/ECSalinity.svg)]()

### EC-Salinity Probe Interface

>Monitor hydroponic nutrient levels or salinity in aquariums, pools, and soil.
* Electrical Conductivity in Siemens
* Total Dissolved Solids in PPM
* Salinity in PSU and PPT
* Temperature in Celsius/Fahrenheit

#### What it is
An I2C sensor device, an optional DS18B20 waterproof temperature probe, and a two-electrode EC probe. It measures the conductivity of a solution and converts it into Siemens (S). From that value, it derives total dissolved solids and salinity. The firmware on the device provides two [calibration options](http://ufire.co/ECSalinity/#calibration), single or dual point which can be used simultaneously. Temperature compensation is also provided in the firmware.

#### Using it
There is extensive [documentation](http://ufire.co/ECSalinity/) on the [specifications](http://ufire.co/ECSalinity/#characteristics), [setup](http://ufire.co/ECSalinity/#getting-started), and [use](http://ufire.co/ECSalinity/#use) of the device. The library to use it is in the Arduino and Particle.io IDE; a python implementation for Raspberry Pi and MicroPython is also available.

~~~
#include <ECSalinity.h>
EC_Salinity ec;
ec.measureEC();
~~~

#### Buy it
Visit [ufire.co](http://ufire.co) and buy a board and probe.
