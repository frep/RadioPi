RadioPi ESP32 program
=====================
The powering of the Hifiberry Amp2 and the raspberry pi is controlled by the [Adafruit HUZZAH32](https://www.adafruit.com/product/3405) feather board. Further, the input control from a rotary encoder, as well as the light from a neopixel stripe is controlled by this esp32 board. 
The pinout of the Huzzah32 feather board can be looked up [here](https://github.com/jango-fx/Adafruit-HUZZAH32-Feather-pinout), and copied from there:

<p align="center">
<img src=pictures/Adafruit-HUZZAH32-Feather-pinout.png width=80%/>
</p>

PlatformIO
----------
The program for the Huzzah32 board was created with the [PlatformIO-IDE](https://platformio.org/) inside of the Visual Studio Code (VSCode).

Setup
-----
After the first startup, the microcontroller doesn't know the wifi credentials. Here comes the **ESPAsyncWiFiManager** library into play. If the microcontroller can't connect to a known wifi, an access point (AP) is launched. Connect to the AP with the name **AutoConnectAP**. A menu as shown in the following picture pops up:

<p align="center">
<img src=pictures/AutoConnectAP.png width=80%/>
</p>

Under **Configure WiFi** the correct wifi can be chosen an the credentials can be entered. From now on, with correct credentials, the microcontroller can connect to the wifi and no AP is launched anymore.

OTA
---
Updating the microcontroller can be a little bit tricky, since it is built into the radio. Luckly, for the esp32 there is the possibility of OTA (Over-The-Air) programming. After the esp32 has succesfully connected to the wifi, a webserver is started with the OTA functionality. The update server can be reached with the following url:
>     http://esp32.local/update

After compiling in the platformIO IDE, the **firmware.bin** file, which has to be uploaded, can be found at the following location (for the MAC enviroment featheresp32-OSX):
>     .pio/build/featheresp32-OSX/firmware.bin
If the hidden directory **.pio** is not shown unter under the macOS, this can be changed with the press **Command + Shift + . (full stop/period)**. 