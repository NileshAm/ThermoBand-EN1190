# Code

## Overview
* <code>main/src/</code> folder contains all the library and header files needed to run the program.
    * Add put header files into its over folder and then put folder in the src file to compile the library at compilation time.
    * **Note**: If files would not be compiled of put anyway else and this behaviour can lead to compilation errors.
* <code>[main.ino](main/main.ino)</code> main file which is uploaded to the ESP.

## Be Aware
1. **Do not** change the codes marked as not to edit as it could break the WIFI programming. And might need to erase the memory and reupload the code from scratch.


## Setting up WIFI Programmer on ESP
1. Reset and clear the ESP flash memory by running
<code>python -m esptool --chip esp8266 erase_flash
</code>.(**Hold down the flash button of the ESP while the command is running.**) [More Info.](https://www.youtube.com/watch?v=7ugtlgoP9Ws)

2. Upload the <code>main.ino</code> file to the ESP through Serial USB connection.
    * Note the IP address of the ESP outputed to the serial monitor.

3. Once restarted, a wifi server will be started in the name of <code>ESP8266_Config</code> connect to the wifi and go to the IP address of the earlier outputed address.

4. Enter the WIFI SSID and Password of the wifi that you what the ESP to connect to.

5. Once restarted, you will see a slow blinking LED and you will be able to access the Serial monitor by opening [WebSocketViewer.html](./main/WebSocketViewer.html). And in arduino IDE you will be able to select the ESP as a WIFI connection.
    * Note : If the ESP does not show up in ports with a wifi signal, try resetting the ESP by pressing the Rest button. And if that also does not work try restarting Arduino IDE.




## Web Serial Monitor Setup
* Open [WebSocketViewer.html](./main/WebSocketViewer.html) in a code editor and change the <code>IP</code> constant to the IP address of the ESP board.
    * Ip address of the board can be found either through your WIFI router or if you set the ESP config through the web portal IP address will be printed on the Serial Moniter when you uploaded the <code>WIFI_Programmer</code> code
* Open the [WebSocketViewer.html](./main/WebSocketViewer.html) in a web browser. After it connects to the server it will output the serial outputs.