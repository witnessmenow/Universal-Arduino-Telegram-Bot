# Universal Telegram Bot Library

An Arduino IDE library for using Telegram Bot API. It's designed to be used with multiple Arduino architectures.

Join the [Arduino Telegram Library Group Chat](https://t.me/arduino_telegram_library) if you have any questions/feedback or would just like to be kept up to date with the library's progress.

## Introduction

This library provides an interface for [Telegram Bot API](https://core.telegram.org/bots/api).

Telegram is an instant messaging service that allows for the creation of bots. Bots can be configured to send and receive messages. This is useful for Arduino projects as you can receive updates from your project or issue it commands via your Telegram app from anywhere.

This is a library forked from [one library](https://github.com/Gianbacchio/ESP8266-TelegramBot) and inspired by [another](https://github.com/CasaJasmina/TelegramBot-Library)

Each library only supported a single type of Arduino and had different features implemented. The only thing that needs to be different for each board is the actual sending of requests to Telegram so I thought a library that additional architectures or boards could be configured easily would be useful, although this springs to mind:

![alt text](https://imgs.xkcd.com/comics/standards.png "standards")


## Installing

The downloaded code can be included as a new library into the IDE selecting the menu:

     Sketch / include Library / Add .Zip library

You also have to install the ArduinoJson library written by [Benoît Blanchon](https://github.com/bblanchon). Search for it on the Arduino Library manager or get it from [here](https://github.com/bblanchon/ArduinoJson).


## Getting Started

To generate your new Bot, you need an Access Token. Talk to [BotFather](https://telegram.me/botfather) and follow a few simple steps described [here](https://core.telegram.org/bots#botfather).


Include UniversalTelegramBot in your project:

    #include <UniversalTelegramBot.h>

and pass it a Bot token and a SSL Client (See the [examples](https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot/tree/master/examples) for more details)

    #define BOTtoken "XXXXXXXXX:XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
    // WiFiSSLClient client; //For 101 boards
    WiFiClientSecure client; //For ESP8266 boards
    UniversalTelegramBot bot(BOTtoken, client);

*NOTE:* This library has not been tested with the 101 boards as I do not have a compatible board. [If you can help please let us know!](https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot/issues/2)

## Features

Here is a list of features that this library covers. (Note: The examples link to the ESP8266 versions)

|Feature|Description|Usage|Example|
|--------|-----------|----|-------|
|*Receiving Messages*|Your bot can read messages that are sent to it. This is useful for sending commands to your arduino such as toggle and LED|`int getUpdates(long offset)` <br><br> Gets any pending messages from Telegram and stores them in **bot.messages** . Offset should be set to **bot.last_message_received** + 1. Returns the numbers new messages received.| [FlashLED](https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot/blob/master/examples/ESP8266/FlashLED/FlashLED.ino) or any other example|
|*Sending messages*|Your bot can send messages to any Telegram or group. This can be useful to get the arduino to notify you of an event e.g. Button pressed etc (Note: bots can only message you if you messaged them first)|`bool sendMessage(String chat_id, String text, String parse_mode = "")` <br><br> Sends the message to the chat_id. Returns if the message sent or not.| [EchoBot](https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot/blob/master/examples/ESP8266/EchoBot/EchoBot.ino#L51) or any other example|
|*Reply Keyboards*|Your bot can send [reply keyboards](https://camo.githubusercontent.com/2116a60fa614bf2348074a9d7148f7d0a7664d36/687474703a2f2f692e696d6775722e636f6d2f325268366c42672e6a70673f32) that can be used as a type of menu.|`bool sendMessageWithReplyKeyboard(String chat_id, String text, String parse_mode, String keyboard, bool resize = false, bool oneTime = false, bool selective = false)` <br><br> Send a keyboard to the specified chat_id. parse_mode can be left blank. Will return true if the message sends successfully.| [ReplyKeyboard](https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot/blob/master/examples/ESP8266/CustomKeyboard/ReplyKeyboardMarkup/ReplyKeyboardMarkup.ino)|
|*Inline Keyboards*|Your bot can send [inline keyboards](https://camo.githubusercontent.com/55dde972426e5bc77120ea17a9c06bff37856eb6/68747470733a2f2f636f72652e74656c656772616d2e6f72672f66696c652f3831313134303939392f312f324a536f55566c574b61302f346661643265323734336463386564613034). <br><br>Note: URLS & callbacks are supported currently|`bool sendMessageWithInlineKeyboard(String chat_id, String text, String parse_mode, String keyboard)` <br><br> Send a keyboard to the specified chat_id. parse_mode can be left blank. Will return true if the message sends successfully.| [InlineKeyboard](https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot/blob/master/examples/ESP8266/CustomKeyboard/InlineKeyboardMarkup/InlineKeyboardMarkup.ino)|
|*Send Photos*|It is possible to send phtos from your bot. You can send images from the web or from the arduino directly (Only sending from an SD card has been tested, but it should be able to send from a camera module)|Check the examples for more info| [From URL](https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot/blob/master/examples/ESP8266/SendPhoto/PhotoFromURL/PhotoFromURL.ino)<br><br>[Binary from SD](https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot/blob/master/examples/ESP8266/SendPhoto/PhotoFromSD/PhotoFromSD.ino)<br><br>[From File Id](https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot/blob/master/examples/ESP8266/SendPhoto/PhotoFromFileID/PhotoFromFileID.ino)|
|*Chat Actions*|Your bot can send chat actions, such as *typing* or *sending photo* to let the user know that the bot is doing something. |`bool sendChatAction(String chat_id, String chat_action)` <br><br> Send a the chat action to the specified chat_id. There is a set list of chat actions that Telegram support, see the example for details. Will return true if the chat actions sends successfully.|
|*Location*|Your bot can receive location data, either from a single location data point or live location data. |Check the example.| [Location](https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot/tree/master/examples/ESP8266/Location/Location.ino)|
|*Channel Post*|Reads posts from channels. |Check the example.| [ChannelPost](https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot/tree/master/examples/ESP8266/ChannelPost/ChannelPost.ino)|
|*Long Poll*|Set how long the bot will wait checking for a new message before returning now messages. <br><br> This will decrease the amount of requests and data used by the bot, but it will tie up the arduino while it waits for messages  |`bot.longPoll = 60;` <br><br> Where 60 is the amount of seconds it should wait | [LongPoll](https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot/tree/master/examples/ESP8266/LongPoll/LongPoll.ino)|

The full Telegram Bot API documentation can be read [here](https://core.telegram.org/bots/api). If there is a feature you would like added to the library please either raise a Github issue or please feel free to raise a Pull Request.


## Other Examples

 Some other examples are included you may find useful:

- BulkMessages : sends messages to multiple subscribers (ESP8266 only).

- UsingWifiManager : Same as FlashLedBot but also uses WiFiManager library to configure WiFi (ESP8266 only).



## License

You may copy, distribute and modify the software provided that modifications are described and licensed for free under [LGPL-3](http://www.gnu.org/licenses/lgpl-3.0.html). Derivatives works (including modifications or anything statically linked to the library) can only be redistributed under [LGPL-3](http://www.gnu.org/licenses/lgpl-3.0.html), but applications that use the library don't have to be.
