/*!
   \file mainpage.md
   \brief doxygen main page
*/
\example basic.ino
This is an example of the basic use of the EC_Salinity class to get an mS measurement.

 \example salinity.ino
This is an example of measuring salinity.

\example dual_point.ino
This is an example of calibrating the probe using dual points.

\mainpage

# Introduction
The EC-Salinity I2C device can measure electrical conductivity in the range of
0.5 - 62mS and salinity from 2 - 42.
<hr>

# Characteristics
- **EC range**: 0.5 - 62mS
- **Salinity range**: 2 - 42
- **Temperature range**: -2 - 35C
- **Interface**: I2C
- **Current use**: ~10mA peak, low idle current ~1mA
- **Voltage range**: 2.7 - 5.5V

It uses the standard Arduino Wire library to interface with the device.
It's operating principle is based on sending a very short-duration DC pulse
from one probe pin and measuring the conductivity on the other pin. The conductivity
is then converted to a temperature compensated Siemen. Salinity is also derived
from the measure and is in PSU, PPT, and PPM.
<hr>

# Connections
Making the connections to the device is as follows:
|   EC Salinity Probe   |   Master device       |
|   :---------------:   |   :---------------:   |
|   GND                 |         GND           |
|   SCL                 |         SCL           |
|   SDA                 |         SDA           |
|   VCC                 |         2.7 - 5V      |

The temperature probe should be connected as follows:
|   EC Salinity Probe   |   Temperature Probe   |
|   :---------------:   |   :---------------:   |
|   +                   |         VCC           |
|   D                   |         Data          |
|   -                   |         GND           |

The EC probe has two pins and can be connected either way.
<hr>

# Setup
When the device is powered for the first time, it will be uncalibrated. The calibration setting
of the probe is referred to as the cell constant (K). To begin the calibration process,
place the thermometer and probe in a calibration solution then call the
#EC_Salinity::calibrateProbe function.

For best results, the probe should be cleaned with distilled water and then placed
in the solution for 5-10 minutes before the probe is used. It shouldn't be
placed on the bottom or side of the solution container.  Note that any
turbidity, air bubbles, large particles, etc will effect readings. An unstable
temperature will decrease accuracy as well.

When calibrating the probe, it is important to consider the expected temperature
range. EC measurements are very temperature dependent, effecting the results by
approximately 2% per degree C. The probe should be calibrated at the median
expected temperature.

Likewise, the probe should be calibrated in the median expected EC range. For example,
if you are planning to measure the salinity of an aquarium, you might expect
readings ±5 from 35PPT (equivalent to 53mS). So a 53mS solution for calibration
would be appropriate, whereas a hydroponic calibration might be 2.77mS.  

Another consideration is the placement of the probe. When the probe sends out a
pulse of electricity, it leaves the probe in 360 degrees. If it is near a metal
surface, you will experience fringing effects. The probe should be calibrated
in an environment as similar as possible to the location it will be deployed in.

#EC_Salinity::setK can be used to set an arbitrary K value.
#EC_Salinity::getK can be used to return the current K value.

#### Dual Point Calibration
Once the probe is calibrated, you can optionally use dual point calibration
for more accurate results between two predesignated points. Two calibration solutions
are required, the low and high values you expect to measure between.

1. Determine the lowest and highest measurement you expect in mS. For example, to
measure salinity in an aquarium, the lowest level you would expect to measaure might
be 30 PPT and the highest might be 38 PPT. These points are referred to as
referenceLow and referenceHigh

2. Using the calibrated probe and a calibration solution at referenceLow, call
#EC_Salinity::measureEC and see what the actual reading is. Do the same for
referenceHigh. These readings are referred to as readingLow and readingHigh.
As an example, the readingLow might be 27 and readingHigh might be 40.

3. Call #EC_Salinity::setDualPointCalibration to save the values to the device, then enable
dual point calibration. By default, the device does not use dual point. A call
to #EC_Salinity::useDualPoint must be made to enable it.
~~~~
  ec.setDualPointCalibration(30, 38, 27, 40);
  ec.useDualPoint(true);
~~~~

#EC_Salinity::calibrateProbeLow and #EC_Salinity::calibrateProbeHigh can be used
to programatically determine the values.

#EC_Salinity::getCalibrateHigh and #EC_Salinity::getCalibrateLow can be used
to get the currently set reference values.

4. Each call to #EC_Salinity::measureEC will use the provided numbers to adjust
the reading. It can be disabled by
~~~~
  ec.useDualPoint(false);
~~~~
<hr>

# Probe Selection
A basic, but adequate, probe is included with the device. It will work reliably
and accurately within the specifications listed above, however it is not specifically
designed to be an EC probe. A professional probe can cost up to several hundred
dollars. If you need the advantages of such an accurate device, this hobby-level
project is not appropriate for you. However, most people don't need to measure
EC to within 0.1uS reliably every time, in fact, a relatively wide range in
accuracy will still be quite acceptable for many use-cases.

Any 2-electrode probe can be attached to the device. Keep in mind
that metal, water, and electricity don't typically cooperate. Things that effect the lifespan
include the chemicals it is exposed to, if it is occasionally exposed to air,
the number of measurements taken, and the types of metal used.
Deposit buildup will effect readings as well as chemical reactions that take
place on the probe surface. It is very likely that it will require regular
maintenance in the form of cleaning or recalibration.
<hr>
# Use
Once the probe has been calibrated, a reading can be taken. The device will use
the most recent temperature measurement obtained from #EC_Salinity::measureTemp.
If there is no temperature reading, a call to #EC_Salinity::measureTemp will be made.
The probe should be placed in the solution to be measured and #EC_Salinity::measureEC called to
start a measurement. For best results, the probe should be cleaned with distilled
water and left in the solution for 5 - 10 minutes. Be sure no air bubbles or debris
are on the probe points.

After the measurement is taken, the following class variables are updated:

- #EC_Salinity::uS
- #EC_Salinity::mS
- #EC_Salinity::S
- #EC_Salinity::PPM_500
- #EC_Salinity::PPM_640
- #EC_Salinity::PPM_700
- #EC_Salinity::salinityPSU
- #EC_Salinity::salinityPPT
- #EC_Salinity::salinityPPM
- #EC_Salinity::tempC
- #EC_Salinity::tempF

#### Temperature Compensation
If the solution to be measured is seawater, be sure to
use the temperature compensation coefficient #EC_Salinity::tempCoefSalinity,
otherwise use #EC_Salinity::tempCoefEC as the parameter. If another compensation
value is desired, it can be provided as a regular float value.

Readings can also be adjusted to a single temperature. In many modes of use,
it is commonly adjusted to 25C. This can be done with the following:

~~~~
  ec.setTempConstant(25);
~~~~
By default, it is set to 0xFF and the actual temperature is used. To re-enable
compensation, set it back to 0xFF.

#### Accuracy
The accuracy of the device can be adjusted by modifying #EC_Salinity::setAccuracy.
By default, nine measurements are taken. A running median sort is applied and
the middle third of the nine measurements are averaged together to provide the
final result. Any number of measurements can be taken as long as it is evenly
divisible by 3. The least number of measures possible is 3..

~~~~
ec.setAccuracy(15);
~~~~

#### Measurement Time
Each individual EC measurement take 10ms. If the default accuracy is set to 9,
a call to #EC_Salinity::measureEC will return in 90ms. A temperature measurement
takes 750ms.

#### Opearting without a DS18B20 Temperature Sensor
The device can operate without a probe attached. If it detects that the probe
is unattached, it does not modify the #EC_Salinity::tempC value. You can optionally set
~~~~
  ec.tempCompensationConstant = 25;
~~~~
or you can specify your own value prior to calling #EC_Salinity::measureEC.
~~~~
ec.tempC = 22;
ec.measureEC(ec.tempCoefEC);
~~~~
<hr>

# Notes on Measuring Seawater
Salinity can be accurately measured between a range of 2 - 42, between the -2 - 35 C.
-1 is returned if it is not within the above limitations.

# References
- Concept: https://www.element14.com/community/community/design-challenges/vertical-farming/blog/2015/09/13/automated-green-house-blogs-main-page
- Temperature coefficients: http://www.posiva.fi/files/2094/POSIVA-2001-15_Working-report_web.pdf
- EC to PSU conversion: https://www.mwa.co.th/download/file_upload/SMWW_1000-3000.pdf
