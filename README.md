#Universal Telegram Bot Library

An Arduino IDE library for using Telegram Bot API. It's designed to be used with multiple Arduino architectures.

Forked from [ESP8266-TelegramBot](https://github.com/Gianbacchio/ESP8266-TelegramBot) & inspired by [TelegramBot-Library](https://github.com/CasaJasmina/TelegramBot-Library)


## Introduction

This library provides an interface for [Telegram Bot API](https://core.telegram.org/bots/api).

It is possible to define your personal Bot, make it able to read and write messages, receive orders and report data collected from the field.


## Installing

The downloaded code can be included as a new library into the IDE selecting the menu:

     Sketch / include Library / Add .Zip library

You also have to install the ArduinoJson library written by [Benoît Blanchon](https://github.com/bblanchon).
Available [here](https://github.com/bblanchon/ArduinoJson).


## Getting started

View the last released Telegram API documentation at: https://core.telegram.org/bots/api.

To generate your new Bot, you need an Access Token. Talk to [BotFather](https://telegram.me/botfather) and follow a few simple steps described [here](https://core.telegram.org/bots#botfather).

For full details, see "[Bots: An introduction for developers](https://core.telegram.org/bots)".


## Examples

Here are listed some examples to help you to build your own Bot:

- EchoBot : replies echoing your messages.

- FlashLedBot : Reacts to your command switching ON/OFF a GPIO.

- EchoBotWithPost : same as EchoBot but using post to commincate with Telegram.

- CustomKeyboard : Same as FlashLedBot but also uses a replyKeyboard



## License

You may copy, distribute and modify the software provided that modifications are described and licensed for free under [LGPL-3](http://www.gnu.org/licenses/lgpl-3.0.html). Derivatives works (including modifications or anything statically linked to the library) can only be redistributed under [LGPL-3](http://www.gnu.org/licenses/lgpl-3.0.html), but applications that use the library don't have to be.
