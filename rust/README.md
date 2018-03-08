### ISE Probe Interface

> Use any Ion Specific Electrode
* measure pH with automatic temperature compensation
* measure ORP and Eh
* raw mV
* temperature in Celsius
* library can be easily extended for any probe

#### What it is
An I2C slave device that can interface with any voltage based sensor such as a pH or ORP probe. There are pin headers available to connect a DS18B20 temperature sensor for automatic temperature compensation. The firmware on the device provides two calibration options, single or dual point.

It is designed to allow multiple probes to be connected to one controlling device and not interfere with each other. For example, a pH and ORP probe can be connected to the same master device and submerged in the same solution without causing cross probe interference.

#### Using it
There is extensive in the source on the use and setup of the device. The library to use it is in the Arduino IDE, Particle.io IDE and a python implementation for Raspberry Pi and MicroPython is also available.
