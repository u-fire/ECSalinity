### EC Salinity Probe Interface for Espruino
This library was developed on the ESP32 platform using a generic board. The library will work with an ESP8266, however there isn't enough memory to run it as-is. You will need to pare it down according to what you are and aren't using.

Visit the [Espruino](http://www.espruino.com/Download) site and download the firmware. Open the archive and pick the appropriate firmware. Open the README for the commands to flash the board. If the README mentions esptool, it can be installed with `pip install esptool`.

- Install the [Espruino Web IDE](http://www.espruino.com/Quick+Start) and open it.
- Connect to your board by clicking the icon in the upper left of the screen and choosing the serial port.
- Copy the code from ec.js, found in the same directory as this README file, into the textbox on the right
- Click the button in the center of the screen to 'Send to Espruino'. The console REPL will display the output on the left.

Please submit Issues or Pull Requests for any problems or fixes.
