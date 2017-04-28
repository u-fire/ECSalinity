/*!
   \file mainpage.md
   \brief doxygen main page
*/
\example basic.ino
This is an example of the basic use of the EC_Salinity class to get an mS measurement.

 \example salinity.ino
This is an example of measuring salinity.

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
- **Current use**: 10mA peak, low idle current ~1mA
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
|   VCC                 |         3 - 5V        |

The temperature probe should be connected as follows:
|   EC Salinity Probe   |   Temperature Probe   |
|   :---------------:   |   :---------------:   |
|   +                   |         VCC           |
|   D                   |         Data          |
|   -                   |         GND           |

The EC probe has two pins and can be connected either way.
<hr>

# Setup
When the device is powered for the first time, it will be uncalibrated and must
be calibrated to provide accurate readings. The calibration setting
of the probe is referred to as the cell constant and represented by
#EC_Salinity::K. An uncalibrated device will show K = -1. To begin the calibration process,
place the thermometer and probe in a calibration solution then call the
#EC_Salinity::calibrateProbe function.

For best results, the probe should be cleaned with distilled water and then placed
in the solution for 5-10 minutes before the probe is used. Note that any
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
surface, you will experience fringing effects. Generally, if the probe is calibrated
where it is to be used, fringing effects are not a concern, but if you calibrate
the probe in, for example, a small cup of plastic solution, then place it in a solution that
has a large amount of interference in the form of metal or electrical activity,
the measurements will vary widely.

#### Dual Point Calibration
Once the probe itself is calibrated, you can optionally use dual point calibration
for more accurate results between two predesignated points. Two calibration solutions
are required for this calibration, the low and high values you expect to measure between.

1. Determine the lowest and highest measurement you expect in mS. For example, to
measure salinity in an aquarium, the lowest level you would expect to measaure might
be 30 PPT and the highest might be 38 PPT. These points are referred to as
referenceLow and referenceHigh

2. Using the calibrated probe and a calibration solution at referenceLow, call
#EC_Salinity::measureEC and see what the actual reading is. Do the same for
referenceHigh. These readings are referred to as readingLow and readingHigh.
As an example, the readingLow might be 27 and readingHigh might be 40.

3. Call #EC_Salinity::setDualPoint
~~~~
  ec.setDualPoint(30, 38, 27, 40);
~~~~

4. Each call to #EC_Salinity::measureEC will use the provided numbers to adjust
the reading. It can be disabled by
~~~~
  ec.useDualPointCalibration =  false;
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
Once the probe has been calibrated, a reading can be taken. The probe should
be placed in the solution to be measured and #EC_Salinity::measureEC called to
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
- #EC_Salinity::K

#### Temperature Compensation
If the solution to be measured is seawater, be sure to
use the temperature compensation coefficient #EC_Salinity::tempCoefSalinity,
otherwise use #EC_Salinity::tempCoefEC as the parameter. If another compensation
value is desired, it can be provided as a regular float value.

Temperature compensation can be disabled by setting #EC_Salinity::useTempCompensation
to false.
Readings can also be adjusted to a single temperature. In many modes of use,
it is commonly adjusted to 25C. This can be done with the following:

~~~~
  ec.tempCompensationConstant = 25;
~~~~
At initialization, it is set to -1 and the actual temperature is used. To re-enable
compensation, set it back to -1.

#### Accuracy
The accuracy of the device can be adjusted by modifying #EC_Salinity::accuracy.
By default, it is set to what is considered high accuracy (EC_HIGH_ACCURACY). Nine measurements are
taken. A running median sort is applied and the middle third of the nine
measurements are averaged together to provide the final result. Any number of
measurements can be taken as long as it is evenly divisible by 3. The least number of
measures possible is 3, as defined by EC_LOW_ACCURACY.

~~~~
ec.accuracy = 15;
~~~~

#### Measurement Time
Each measurement takes approximately 1 second. A majority of the time is spent
waiting for the temperature measurement to finish. The EC portion of the measurement
is done extremely quickly to avoid electrolysis effects.
<hr>

# Notes on Measuring Seawater
Salinity can be accurately measured between a range of 2 - 42, between the -2 - 35 C.
-1 is returned if it is not within the above limitations.

# References
- Concept: https://www.element14.com/community/community/design-challenges/vertical-farming/blog/2015/09/13/automated-green-house-blogs-main-page
- Temperature coefficients: http://www.posiva.fi/files/2094/POSIVA-2001-15_Working-report_web.pdf
- EC to PSU conversion: https://www.mwa.co.th/download/file_upload/SMWW_1000-3000.pdf
