# TemperatureQueryDisplay

This is a DIY remote temperature sensor consisting of multiple parts

1. Temperature sensor module
2. Web server enabled device
3. User interface and web server

The first prototype implementation consists of an [LM335](https://www.ti.com/product/LM335) temperator sensor mounted on an [Arduino Pro Mini](https://www.arduino.cc/en/Main/ArduinoBoardProMini) soldered onto a carrier board that plugs into a [(WeMos module) ESP-8266 with OLED and 18650 battery device](https://wiki.geekworm.com/WEMOS_ESP8266_Board_with_OLED_%26_18650_Battery_Holder).  The ESP8266 connects to the local Wi-Fi and serves as a web server with a GET /temp endpoint.

The user interface is built on [Angular](https://angular.io/) and is built to a set of html/css/js/etc. files which can be served from any web server.  These files can be served locally using "ng serve" for development purposes, or are also deployed to github.io for serving from there.

The user interface queries the remote temperature device and presents the temperature in the user's web browser.  One use case is having the remote temperature device in one area of the house, and monitor the temperature via smart phone.   The temperature can be monitored by multiple household
members, and multiple devices can be monitored too.

    +------+         +------+
    | 72°F |  ~~~~   | 72°F |  
    +------+         |------|
                     | 55°F |
    +------+         |------|
    | 55°F |         | 40°F |
    +------+         +------+
                  ~   phone
    +------+    ~
    | 40°F |  ~
    +------+
     remote
     sensors

The design builds on the [single-responsibility](https://en.wikipedia.org/wiki/Single-responsibility_principle) and [KISS](https://en.wikipedia.org/wiki/KISS_principle) principles.

* The temperature sensor board is responsible for acquiring and reporting the temperature.  In the first prototype it uses a simple UART connection, reporting the temperature at an interval of about one second.
* The remote WiFi board is responsible for serving a web endpoint (GET /temp).  It receives the data from the attached sensor board, and serves external requests.
* The user interface running in a mobile web browser retrieves the temperature data over Wi-Fi and presents the data to the end user.  Data is refreshed appropriately (e.g. 15 seconds).

The specific details of how each component was built is not as relevant as the communication details of retrieving the sensor data remotely, and that it is presented to the user.  Each detail in the solution is rather trivial, the success is integrating the multiple features, and the lasting value is the functionality as a whole.

The general nature of the communication interfaces provides a open system with various options possible.  Any component can be rewritten and replaced.  The temperature sensor could be integrated more directly into the remote server.  The user interface could be rewritten with any framework that allows a GET /temp request receiving a JSON payload.

    { 
        "raw": 609,
        "fahrenheit": 67 
    }

The raw value is currently there for diagnostics and calibration.  It is the analog value read from the temperature sensor that is transformed into the desired temperature units.   Only the fahrenheit value is expected to be there (except celsius will also be added soon).   Other temperature modules may return data in different means.

The Arduino prototype module sends its data every second over its serial connection in the form

    raw=609
    fahrenheit=67

This data is easy to parse, before returning as JSON by the remote web server.

I am most grateful for the ESP8266 module which can run a web server, and the Arduino IDE and available libraries for compiling C++ software for both the Arduino Pro Mini (ATMEGA328P) and ESP8266 (D1) targets.  Angular web development is pretty great too, focused on using HTML/CSS templates with TypeScript code.

The prototype temperature module is an LM335 soldered directly onto an Arduino Pro Mini.  LM335 has 3 pins (1-calibrate, 2-vout, 3-gnd).  Pin 1 is left unconnected, pin 3 is grounded, pin 2 is connected to A6 and a 2.2K resistor is dead bugged from pin 2 and A6 to 5V(VCC).  The carrier board has a plug for connecting to the ESP8266 board, with ground and 5V connected as appropriate, and the Arduino's TX pin connected to ESP8266 RX pin labeled D9 on this board.  Yes, I've connected a 5V module directly to a 3V3 ESP8266.  The ESP8266 is tolerant of such a connection while it must be powered by 3V3 itself.  And the Arudino Pro Mini is powered by the boost circuit present on this ESP8266/OLED/18650 board.  Note that OLED support requires inclusion of the [SSD1306 library from ThingPulse](https://github.com/ThingPulse/esp8266-oled-ssd1306).

Android app development (via [Capacitor](https://capacitorjs.com/solution/angular)) is in progress (doesn't work yet but should soon).