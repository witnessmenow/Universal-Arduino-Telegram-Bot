#ESP32 - LED button update

This is an example of how one can update inline keyboard messages (buttons).
The message_id of the specific message is sent when a message is received.
This message_id can be used to UPDATE that message.
One can update text inside a message, but also buttons can be updated.
This way one can build menu's, like the menu the botfather uses.

In this simple example we use a inlinekeyboard button to toggle (and update) the state of a LED.

NOTE: You will need to enter your SSID, password and bot Token for the example to work.

Example and update to Universal-Arduino-Telegram-Bot originally written by 
[Frits Jan van Kempen] (https://github.com/fritsjan) with inspiration from [RomeHein] (https://github.com/RomeHein)

Adapted by [Brian Lough](https://github.com/witnessmenow)

## License

You may copy, distribute and modify the software provided that modifications are described and licensed for free under [LGPL-3](http://www.gnu.org/licenses/lgpl-3.0.html). Derivatives works (including modifications or anything statically linked to the library) can only be redistributed under [LGPL-3](http://www.gnu.org/licenses/lgpl-3.0.html), but applications that use the library don't have to be.
