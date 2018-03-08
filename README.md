[![GitHub release](https://img.shields.io/github/release/u-fire/ECSalinity.svg)]()
[![Codacy grade](https://img.shields.io/codacy/grade/981e93b4cd0c44f4b9db629b1c393ecf.svg)]()

### EC_Salinity Probe Interface

>Measure Electrical Conductivity in Siemens, Total Dissolved Solids in PPM, and Salinity in PSU and PPT. Monitor hydroponic nutrient levels or salinity in aquariums, pools, and soil.
* Electrical Conductivity in Siemens
* Total Dissolved Solids in PPM
* Salinity in PSU and PPT

Available on [Tindie](https://www.tindie.com/products/ufire/ec-salinity-probe-interface/)    
or make one with the [source code](https://www.github.com/u-fire/ec-salinity-probe) and [schematics](https://upverter.com/ufire/6cab745dd03f6f6d/EC-Salinity-Probe-2018a/)  

#### What it is
An ATTiny85 programmed as an I2C slave, a DS18B20 waterproof temperature probe, and a two-electrode EC probe. It measures conductance and converts it into a temperature-compensated SI derived unit called a Siemens. From that value, it determines TDS and salinity. Any two-electrode probe can be used.

#### Using it
There is extensive [documentation](http://ufire.co/ECSalinity/) on the use and setup of the device. The library is available for download and installation in the Arduino Library Manager, PlatformIO Library Manager, and Particle.io IDE. There are also python implementations for Raspberry Pi and MicroPython.
