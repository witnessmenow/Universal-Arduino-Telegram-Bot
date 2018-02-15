## ESP32 - AdvancedLED PlatformIO
PlatformIO example for controlling an LED from a Telegram Bot.
This example sets up an ESP32 to be controlled through a Telegram Bot.
As of January 3rd, 2018, the platform (in platformio.ini) must be set to espressif32_stage.

#### Pre-requisites
* You must have a Telegram Bot for this example to work. To make one,
  1. Open Telegram (on mobile, web, or desktop)
  2. Start a chat with BotFather (@BotFather)
  3. Send `/start` to BotFather, followed by `/newbot`
  4. Send a friendly name for your bot (this isn't the username of bot)
  5. Type in and send the username for your bot (ending in bot)
  6. Copy the token provided by BotFather and paste it at BOTtoken below
* Modify `ssid` and `password` to match your WiFi network settings
* Modify `BOTtoken` with the correct bot token acquired from Botfather
* Modify `ledPin` to match the GPIO pin on which the LED on your board is connected

#### Available commands for the bot:
* /start - Displays an intro message in chat and provides a custom keyboard with available commands
* /options - Displays a custom keyboard with available commands
* /ledon - Turns on the LED specified by `ledPin`
* /ledoff - Turns off the LED specified by `ledPin`
* /blink - Starts blinking the LED specified by `ledPin`
* /PWM - Provides an inline keyboard for setting a PWM value for the LED specified by `ledPin`

#### Notes
* Clicking on a button on an inline keyboard sends back a message of type="callback_query".
* Blink is implemented using low frequency PWM, as fetching message updates can take enough time to cause a noticeable lag in blinking period.
* LED implementation is based on inverse polarity of LED.
* Telegram Bot API documentation available at https://core.telegram.org/bots/api

## Credit
Application originally written by [Giancarlo Bacchio](giancarlo.bacchio@gmail.com) for [ESP8266-TelegramBot library](https://github.com/Gianbacchio/ESP8266-TelegramBot)
Adapted by [Brian Lough](https://github.com/witnessmenow) for UniversalTelegramBot Library
Adapted for ESP32, and modified for advance features by [Pranav Sharma](https://github.com/pro2xy)

## License

You may copy, distribute and modify the software provided that modifications are described and licensed for free under [LGPL-3](http://www.gnu.org/licenses/lgpl-3.0.html). Derivatives works (including modifications or anything statically linked to the library) can only be redistributed under [LGPL-3](http://www.gnu.org/licenses/lgpl-3.0.html), but applications that use the library don't have to be.
