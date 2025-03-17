# Design
## Overview
<span style="font-size:1.2em">This contains all the design specifications and design files of the product.</span>

* Break out board must be connected in a way that **wifi chip is facing the LED**, and USB terminal is on the free end.
    * ***Note : Make sure the ```Vin``` and ```GND``` pins line up with the 2 female header pins.***
* Vin must be supplied with 5V.
* All buttons as connected to ```GND``` on free end.(Except ```D0``` button).
* As RGB LED is a commom anode LED its common terminal is connected to Vin. When power through USB or other wise LED might not turn on as Vin would not be powered.

## Circuit
### Pin out
1. **D0 (GPIO16) :**  
    * Can be connected to either push button 3 for the reset depending on the requirement.

        1.1. **Button Mode :** Put the jumper to the top position. (*Only configurable as a pull down button.*)

        1.2. **Wake Mode :** Put the jumper tot the lower position.(*Then pin is connected to RST pin and can be used to wake the ESP from deep sleep.*)


2. **D1 (GPIO5) :**  
    * Connected to the middle push button.
    * Internally pulled up (to 3.3V) by code.

3. **D2 (GPIO4) :**  
    * Connected to the **DS18B20** temperature sensor.
    * There is a 3.9 kΩ resistor to 3.3V

4. **D3 (GPIO0) :** 
    * Connected to the middle push button.
    * Internally pulled up (to 3.3V) by code.
    * **Note : *As this pin is also connected to flash pin of ESP make sure not to push while booting up.***
5. **D4 (GPIO2) :**
    * Pin of the built in LED.
    * Connecting any thing might lead to built in LED not operating as intented.
6. **3V & GND :**
    * In this board there seem that these 2 pinsare not connected or mulfunctioning.
7. **D5 (GPIO14) :**
    * Connected to the **Red** pin of the RGB LED with a 220Ω resistor.
    * As the LED is a common anode LED, pin must be set to ```LOW``` to light the LED.
8. **D6 (GPIO12) :**
    * Connected to the **Blue** pin of the RGB LED with a 220Ω resistor.
    * As the LED is a common anode LED, pin must be set to ```LOW``` to light the LED.
9. **D7 (GPIO13) :**
    * Connected to the **Green** pin of the RGB LED with a 220Ω resistor.
    * As the LED is a common anode LED, pin must be set to ```LOW``` to light the LED.
10. **D8 (GPIO15) :**
    * As its a boot critical pin avoid connecting anything to this pin.
11. **RST Pin:**
    * Depending on jumper position either connected to ```D0``` or not connected at all. Refer ```D0``` pinout for more info.
