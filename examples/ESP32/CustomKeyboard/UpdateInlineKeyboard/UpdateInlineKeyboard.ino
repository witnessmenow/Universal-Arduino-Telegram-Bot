/*******************************************************************
    An example to show how to edit an existing inline keyboard.

    In this example the keyboard is updated with the state of
    the LED.
    
    written by Frits Jan van Kempen
 *******************************************************************/
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

// Initialize Wifi connection to the router
const char *ssid = "mySSID";
const char *password = "myPASSWORD";

// Initialize Telegram BOT
#define BOTtoken "xxxxxxxxxx:xxxxxxxxxxxxxxxxxxxxxxxxxx" // your Bot Token (Get from Botfather)
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

int Bot_mtbs = 1000; //mean time between scan messages
long Bot_lasttime;   //last time messages' scan has been done
int last_message_id = 0;

// LED parameters
const int ledPin = 2; // Internal LED on DevKit ESP32-WROOM (GPIO2)
int ledState = LOW;

void handleNewMessages(int numNewMessages)
{

    for (int i = 0; i < numNewMessages; i++)
    {

        // Get all the important data from the message
        int message_id = bot.messages[i].message_id;
        String chat_id = String(bot.messages[i].chat_id);
        String text = bot.messages[i].text;
        String from_name = bot.messages[i].from_name;
        if (from_name == "")
            from_name = "Guest";
        String msg = ""; // init a message string to use

        // Output the message_id to give you feeling on how this example works
        Serial.print("Message id: ");
        Serial.println(message_id);

        // Inline buttons with callbacks when pressed will raise a callback_query message
        if (bot.messages[i].type == "callback_query")
        {
            Serial.print("Call back button pressed by: ");
            Serial.println(bot.messages[i].from_id);
            Serial.print("Data on the button: ");
            Serial.println(bot.messages[i].text);

            if (text == "/toggleLED")
            {

                // Toggle the ledState and update the LED itself
                ledState = !ledState;
                digitalWrite(ledPin, ledState);

                // Now we can UPDATE the message, lets prepare it for sending:
                msg = "Hi " + from_name + "!\n";
                msg += "Notice how the LED state has changed!\n\n";
                msg += "Try it again, see the button has updated as well:\n\n";

                // Prepare the buttons
                String keyboardJson = "["; // start Json
                keyboardJson += "[{ \"text\" : \"The LED is ";
                if (ledState)
                {
                    keyboardJson += "ON";
                }
                else
                {
                    keyboardJson += "OFF";
                }
                keyboardJson += "\", \"callback_data\" : \"/toggleLED\" }]";
                keyboardJson += ", [{ \"text\" : \"Send text\", \"callback_data\" : \"This text was sent by inline button\" }]"; // add another button
                //keyboardJson += ", [{ \"text\" : \"Go to Google\", \"url\" : \"https://www.google.com\" }]"; // add another button, this one appears after first Update
                keyboardJson += "]"; // end Json

                // Now send this message including the current message_id as the 5th input to UPDATE that message
                bot.sendMessageWithInlineKeyboard(chat_id, msg, "Markdown", keyboardJson, message_id);
            }

            else
            {
                // echo back callback_query which is not handled above
                bot.sendMessage(chat_id, text, "Markdown");
            }
        }

        // 'Normal' messages are handled here
        else
        {
            if (text == "/start")
            {
                // lets create a friendly welcome message
                msg = "Hi " + from_name + "!\n";
                msg += "I am your Telegram Bot running on ESP32.\n\n";
                msg += "Lets test this updating LED button below:\n\n";

                // lets create a button depending on the current ledState
                String keyboardJson = "["; // start of keyboard json
                keyboardJson += "[{ \"text\" : \"The LED is ";
                if (ledState)
                {
                    keyboardJson += "ON";
                }
                else
                {
                    keyboardJson += "OFF";
                }
                keyboardJson += "\", \"callback_data\" : \"/toggleLED\" }]";                                                     //callback is /toggleLED
                keyboardJson += ", [{ \"text\" : \"Send text\", \"callback_data\" : \"This text was sent by inline button\" }]"; // add another button
                keyboardJson += "]";                                                                                             // end of keyboard json

                //first time, send this message as a normal inline keyboard message:
                bot.sendMessageWithInlineKeyboard(chat_id, msg, "Markdown", keyboardJson);
            }
            if (text == "/options")
            {
                String keyboardJson = "[[{ \"text\" : \"Go to Google\", \"url\" : \"https://www.google.com\" }], [{ \"text\" : \"Send\", \"callback_data\" : \"This was sent by inline\" }]]";
                bot.sendMessageWithInlineKeyboard(chat_id, "Choose from one of the following options", "", keyboardJson);
            }
        }
    }
}

void setup()
{
    Serial.begin(115200);

    // Attempt to connect to Wifi network:
    Serial.print("Connecting Wifi: ");
    Serial.println(ssid);

    // Set WiFi to station mode and disconnect from an AP if it was Previously
    // connected
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(500);
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    pinMode(ledPin, OUTPUT);        // initialize ledPin as an output.
    digitalWrite(ledPin, ledState); // initialize pin as low (LED Off)
}

void loop()
{
    // run this in loop to poll new messages
    if (millis() > Bot_lasttime + Bot_mtbs)
    {
        int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

        while (numNewMessages)
        {
            Serial.println("got response");
            handleNewMessages(numNewMessages);
            numNewMessages = bot.getUpdates(bot.last_message_received + 1);
        }

        Bot_lasttime = millis();
    }
}