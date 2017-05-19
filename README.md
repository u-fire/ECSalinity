---
layout: default
---

### EC-Salinity Probe

>Monitor hydroponic nutrient levels, salinity levels in aquariums or the ocean,
saltwater pools, soil salinity, water quality
* Electrical Conductivity in Siemens
* Total Dissolved Solids in PPM
* Salinity in PSU and PPT

![ECSalinity probe](/images/ECSalinity.png)

[Buy one](http://www.tindie.com)    
or make one with the [source code](https://www.github.com/u-fire/ec-salinity-probe) and [schematics](https://upverter.com/justind000/19cb71ec38391a95/EC-Salinity-Probe/).  
You can also have the [board made](http://dirtypcbs.com/store/user/manage/d91w/19cb71ec38391a95_14924452820000_gerber.zip)
and get all the board components at [Mouser](http://www.mouser.com/ProjectManager/ProjectDetail.aspx?AccessID=02223dd686)

#### What it is
An ATTiny85 programmed as an I2C slave, a DS18B20 waterproof temperature probe, and a two-electrode EC probe. It measures conductance and converts it into a temperature-compensated Siemen. From that value, it derives PPM and salinity. It is accurate to ~0.5mS in seawater, and to within ~0.05mS in the hydroponic or freshwater range.

#### Using it
An Arduino-compatible [library](https://github.com/u-fire/ECSalinity) is provided to make using the probe easy and there is extensive [documentation](http://ufire.co/ECSalinity/) on the use and setup of the device.

~~~
#include <ECSalinity.h>
EC_Salinity ec;

mS = ec.measureEC(ec.tempCoefEC);
~~~
