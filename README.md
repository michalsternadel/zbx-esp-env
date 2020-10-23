# zbx-esp-env

Temperature and humidity monitoring for Zabbix using ESP8266 and DHT11 sensors.

[![boardandsensor](https://raw.githubusercontent.com/michalsternadel/zbx-esp-env/master/nodemcuanddht11.png)](https://raw.githubusercontent.com/michalsternadel/zbx-esp-env/master/nodemcuanddht11.png)



## Features

* Displays temperature and huminity from enabled DHT sensors via http
[![sensorswebsite](https://raw.githubusercontent.com/michalsternadel/zbx-esp-env/master/website.png)](https://raw.githubusercontent.com/michalsternadel/zbx-esp-env/master/website.png)

* Displays temperature and huminity from enabled DHT sensors via serial connection
[![serialoutput](https://raw.githubusercontent.com/michalsternadel/zbx-esp-env/master/serial.png)](https://raw.githubusercontent.com/michalsternadel/zbx-esp-env/master/serial.png)

* Provides readings from sensors in json format (http://ESP_IP/sensors)

* Autodiscovery of sensors with predefined alerts (http://ESP_IP/discovery)


## Installation

Use arduino IDE to compile and upload solution to your board.
* Install ESP8266 Board in Arduino IDE. Enter http://arduino.esp8266.com/stable/package_esp8266com_index.json in Additional board manager URL (Settings tab in File->Preferences).

* Go to Boards manager (Tools->Board: "xxx" -> Boards Manager...) and download esp8266 by ESP8266 Community (version 2.7.2 works fine).

* Install additional libraries (Sketch->Include Library->Manage Libraries...) Arduino_JSON, Adafruit Unified Sensors and DHT.

* Install additional libraries by zip package (Sketch->Include Library->Add .ZIP Library...) (Download zip from https://github.com/me-no-dev/ESPAsyncWebServer and https://github.com/me-no-dev/ESPAsyncTCP).

* Change ssid and password in wificonfig.h file.

* Enable sensors in sensors.h file and change their configuration.

* Hook DHT11 sensors to +3.3V, GND and configured data pins.

* Upload sketch to your board.

* Import Zabbix template and add a host to monitoring. The discovery process will get all the sensors and create graphs for you.

* Watch sensors reading anytime from your browser (http://ESP_IP/).

## Some screenshots

[![latestdata](https://raw.githubusercontent.com/michalsternadel/zbx-esp-env/master/latestdata.png)](https://raw.githubusercontent.com/michalsternadel/zbx-esp-env/master/latestdata.png)

[![tempgraph](https://raw.githubusercontent.com/michalsternadel/zbx-esp-env/master/temperaturegraph.png)](https://raw.githubusercontent.com/michalsternadel/zbx-esp-env/master/temperaturegraph.png)




