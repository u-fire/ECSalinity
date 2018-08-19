### EC Salinity Probe Interface for Micropython

The examples only differ from the [documentation](http://ufire.co/ECSalinity/) in the class initialization; `ec = ecsalinity(-1, 5, 4)` creates a new I2C instance using SCL pin 5 and SDA pin 4, -1 indicates software implementation of I2C.

This library was developed on the ESP32 platform using a generic board. There isn't enough memory to run the shell file on an ESP8266. Other platforms haven't been tested.

Visit the [MicroPython](http://micropython.org/download) site and download the firmware.

To get started with the basic example:
- `pip install esptool`
- `pip install adafruit-ampy`
- `esptool.py --port [/dev/ttyUSB0|COM1] erase_flash`
- `esptool.py --chip esp32 --port [/dev/ttyUSB0|COM1] write_flash -z 0x1000 [firmware file]`
- `git clone https://github.com/u-fire/ECSalinity.git --depth=1`
- `cd ECSalinity/python/MicroPython`
- `ampy --port [/dev/ttyUSB0|COM1] put ecsalinity.py`
- `ampy --port [/dev/ttyUSB0|COM1] put basic.py`
- `putty -serial [/dev/ttyUSB0|COM1] -sercfg 115200, 8, n, 1` or other serial program

Within the serial terminal:

`import basic`

To run the shell example:
- `ampy --port [/dev/ttyUSB0|COM1] put cmd.py`
- `ampy --port [/dev/ttyUSB0|COM1] put shell.py`
- `putty -serial [/dev/ttyUSB0|COM1] -sercfg 115200, 8, n, 1` or other serial program

Within the serial terminal

`import shell`

Please submit Issues or Pull Requests for any problems or fixes.
