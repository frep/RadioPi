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