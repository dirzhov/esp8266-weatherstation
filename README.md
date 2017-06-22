# ESP8266 weather station
---

This is a weater station based on ESP8266 module. Sketch can handle different sensors. Currently it is adjusted to use DHT11 sencsor and analog data from photoresitor.
All data is obtained from esp module via ajax requests and will be stored in a browser. Also in Week and Moth tabs appears data for last week and for last month accordingly. Week and month data is stored in the esp's RAM and can be accessible until esp restarted or powered off. Length of data for both charts is 100.

![Screenshot of weather station](https://github.com/dirzhov/esp8266-weatherstation/raw/master/weatherstation.png "Screenshot of weather station")

All files for web application are stored in the web folder. You can add/edit files in development. Use index.htm file in developement instead of index.html. After all changes made in the index.htm copy all BODY's html code into the index.html To reflect all changes in the web app on ESP module need to execute deploy-web.bat. This script minifies all not minified files and gzips them into the data folder. To upload web app's files from data folder use [this](https://github.com/esp8266/arduino-esp8266fs-plugin) plugin for Arduino IDE.
