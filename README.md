### EC-Salinity Probe

Monitor hydroponic nutrient levels, salinity levels, water quality, etc.

[Buy one](http://www.tindie.com)    
or try to make one with the [Source code](https://www.github.com/u-fire/ec-salinity-probe) and [Schematics](https://upverter.com/justind000/19cb71ec38391a95/wq/)  

#### What it is...
An ATTiny85 as an I2C slave, a DS18B20 waterproof temperature probe, and a two-plate EC probe. It measures conductance and converts it into a temperature-compensated Siemen. From that value, it derives PPM and salinity. It is accurate to ~0.5mS in seawater, and to within ~0.05mS in the hydroponic or saltwater-pool range. 

#### Using it
An Arduino-compatible [library](https://github.com/u-fire/ECSalinity) is provided to make using the probe easy and there is extensive [documentation](http://ufire.co/ECSalinity/) on the use and setup of the device.

~~~
#include <ECSalinity.h>
EC_Salinity ec;

mS = ec.measureEC(ec.tempCoefEC);
~~~
