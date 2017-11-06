#ESP8266 - Echo Bot Webhook

This is a basic example of how to use UniversalTelegramBot with ___Webhook technology___ on ESP8266 based boards.

The application will echo an message it received back to the user.

NOTE: 

You will need to:

* enter your SSID, 
* password,
* bot Token, 

In opposite to standart long poll usage, you also need to use Webhook version:

* generate your SSL private key and certificate;
* change update method for your Telegram bot to Webhook with your certificate and secret URI;
* update your ESP library to version 2.5.0 (in OCtober 2017 not released yet) (we need to use class WiFiServerSecure)

for the example to work.

Application originally written by [Giancarlo Bacchio](giancarlo.bacchio@gmail.com) for [ESP8266-TelegramBot library](https://github.com/Gianbacchio/ESP8266-TelegramBot)

Adapted by [Brian Lough](https://github.com/witnessmenow)

Webhook technology by [Denis G Dugushkin](https://github.com/denzen84)

## License

You may copy, distribute and modify the software provided that modifications are described and licensed for free under [LGPL-3](http://www.gnu.org/licenses/lgpl-3.0.html). Derivatives works (including modifications or anything statically linked to the library) can only be redistributed under [LGPL-3](http://www.gnu.org/licenses/lgpl-3.0.html), but applications that use the library don't have to be.
