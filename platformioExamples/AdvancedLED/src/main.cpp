/*******************************************************************
Example for controlling an LED from a Telegram Bot

You must have a Telegram Bot for this example to work. To make one,
1. Open Telegram (on mobile, web, or desktop)
2. Start a chat with BotFather (@BotFather)
3. Send /start to BotFather, followed by /newbot
4. Send a friendly name for your bot (this isn't the username of bot)
5. Type in and send the username for your bot (ending in bot)
6. Copy the token provided by BotFather and paste it at BOTtoken below

Telegram Bot API documentation available at https://core.telegram.org/bots/api

Note: As of 3rd Jan. 2018, it is necessary to use espressif32_stage
platform for PlatformIO

written by Giacarlo Bacchio (Gianbacchio on Github)
adapted by Brian Lough ( witnessmenow ) for UniversalTelegramBot library
adapted by Pranav Sharma ( PRO2XY ) for ESP32 on PlatformIO
Library related discussions on https://t.me/arduino_telegram_library

*******************************************************************/
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>


// WiFi parameters
const char *ssid = "SSID";     // your network SSID (name)
const char *password = "PASS"; // your network key

// Bot parameters
#define BOTtoken "xxxxxxxxxx:xxxxxxxxxxxxxxxxxxxxxxxxxx"  // your Bot Token (Get from Botfather)
int Bot_mtbs = 1000; //mean time between scan messages
long Bot_lasttime;   //last time messages' scan has been done

// LED parameters
const int ledPin = 16; // Internal Red LED on Karyni (2 on NodeMCU-32S)
enum ledModes {ON = 0, OFF, BLINK, PWM}; // define modes of operation for LED (inverse polarity)
enum ledModes ledmode = OFF; //  Start with off

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// Function prototypes (PlatformIO doesn't make these for you automatically)
void handleNewMessages(int numNewMessages); // parses new messages and sends them to msgInterpretation
void msgInterpretation(String from_name, String text, String chat_id, String message_type);


void setup() {
  Serial.begin(115200);
  //bot._debug=true; // uncomment to see debug messages from bot library

  // attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  // Start WiFi in Station mode
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // Wait for WiFi to connect
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(500);
  }

  Serial.println("\r\nConnected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  pinMode(ledPin, OUTPUT); // initialize ledPin as an output.
  digitalWrite(ledPin, HIGH); // initialize pin as high (LED Off)
}

void loop() {
  // Every "Bot_mtbs" the bot checks if any messages have arrived
  if (millis() > Bot_lasttime + Bot_mtbs)  {
    Bot_lasttime = millis();
    Serial.print(F("Checking for messages.. "));
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    Serial.print(numNewMessages); Serial.println(" new messages");

    if(numNewMessages > 0){
      handleNewMessages(numNewMessages);
    }
  }
}

// Parse new messages and send them for interpretation
void handleNewMessages(int numNewMessages) {
  // Extract info from the message
  for (int i = 0; i < numNewMessages; i++) {
    Serial.print(F("Handling message ")); Serial.println(i+1);
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;

    String from_name = bot.messages[i].from_name;
    if (from_name == "") from_name = "Guest";
    // Call the function for understand the message
    msgInterpretation(from_name, text, chat_id, bot.messages[i].type);
  }
}

void msgInterpretation(String from_name, String text, String chat_id, String message_type) {
  Serial.print(F("Interpreting message: ")); Serial.println(text);
  Serial.print(F("Type: ")); Serial.println(message_type);
  Serial.print(F("From: ")); Serial.println(from_name);

  if (text == "/start") { // First interaction of user
    String welcome = "Hi " + from_name + "!\n";
    welcome += "I am your Telegram Bot running on ESP32.\n\n";
    welcome += "Select one of the /options below!\n\n";
    String keyboardJson = "[[\"/ledon\", \"/ledoff\"],[\"/blink\", \"/PWM\"],[\"/status\"]]";
    bot.sendMessageWithReplyKeyboard(chat_id, welcome, "Markdown", keyboardJson, true);
  }

  if (text == "/options") { // List out the custom keyboard
    String keyboardJson = "[[\"/ledon\", \"/ledoff\"],[\"/blink\", \"/PWM\"],[\"/status\"]]";
    bot.sendMessageWithReplyKeyboard(chat_id, "Choose from one of the following options", "", keyboardJson, true);
  }

  if (text == "/status") {  // Report present ledmode to user
    String response = "LED is ";
    switch (ledmode){
      case ON:
        response += "on";
        break;
      case OFF:
        response += "off";
        break;
      case BLINK:
        response += "blinking";
        break;
      case PWM:
        response += "PWMing";
        break;
    }
    bot.sendMessage(chat_id, response, "");
  }

  if (text == "/ledon") {
    ledmode = ON;
    ledcDetachPin(ledPin); // detach pin from ledc
    digitalWrite(ledPin, LOW); // turn the LED on (drive pin LOW)
    Serial.println(F("Turning LED on"));
    bot.sendMessage(chat_id, "Turning LED on", "");
  }

  if (text == "/ledoff") {
    ledmode = OFF;
    ledcDetachPin(ledPin); // detach pin from ledc
    digitalWrite(ledPin, HIGH); // turn the LED off (drive pin HIGH)
    Serial.println(F("Turning LED off"));
    bot.sendMessage(chat_id, "Turning LED off", "");
  }

  if (text == "/blink") {
    ledmode = BLINK;
    // We use ledc for blinking by setting PWM at a low frequency
    ledcSetup(0, 1, 8); // Channel, Freq., Resolution
    ledcAttachPin(ledPin, 0); // Pin, Channel
    ledcWrite(0, 192); // Channel, Duty (stay off for 90% time) (remember inverse polarity!)
    Serial.println(F("Blink set"));
    bot.sendMessage(chat_id, "Blink set", "");
  }

  if (text == "/PWM") { // Send an inline keyboard for seleting PWM values (percentage)
    // For inline keyboard markup, see https://core.telegram.org/bots/api#inlinekeyboardmarkup
    String keyboardJson = "[[{ \"text\" : \"0\", \"callback_data\" : \"0\" }],";
    keyboardJson += "[{ \"text\" : \"10\", \"callback_data\" : \"10\" },";
    keyboardJson += "{ \"text\" : \"20\", \"callback_data\" : \"20\" },";
    keyboardJson += "{ \"text\" : \"30\", \"callback_data\" : \"30\" }],";
    keyboardJson += "[{ \"text\" : \"40\", \"callback_data\" : \"40\" },";
    keyboardJson += "{ \"text\" : \"50\", \"callback_data\" : \"50\" },";
    keyboardJson += "{ \"text\" : \"60\", \"callback_data\" : \"60\" }],";
    keyboardJson += "[{ \"text\" : \"70\", \"callback_data\" : \"70\" },";
    keyboardJson += "{ \"text\" : \"80\", \"callback_data\" : \"80\" },";
    keyboardJson += "{ \"text\" : \"90\", \"callback_data\" : \"90\" }],";
    keyboardJson += "[{ \"text\" : \"100\", \"callback_data\" : \"100\" }]]";
    Serial.println(F("Sending PWM keyboard"));
    bot.sendMessageWithInlineKeyboard(chat_id, "Set PWM level", "", keyboardJson);
  }

  if(message_type=="callback_query") { // Received when user taps a button on inline keyboard
    // In our case, callback_query is only received for PWM values. In other cases you may
    // want to append an identifier to the values sent in 'callback_data' (e.g. 'duty=10')
    // and then check for it here using text.startsWith("duty=") or something similar.
    ledmode = PWM; // set proper LED mode
    uint8_t duty = text.toInt();  // Convert value to int
    duty = duty*2.55; // For duty between 0 - 100%, actual duty would be between 0 - 255

    // We use ledc for PWM
    ledcSetup(0, 5000, 8); // Channel, Freq., Resolution
    ledcAttachPin(ledPin, 0); // Pin, Channel
    ledcWrite(0, 255-duty); // Channel, Duty (255 - x to inverse polarity)
    Serial.println(F("PWM set"));
    String message = "PWM set with duty ";
    message += String(duty);
    bot.sendMessage(chat_id, message, "");
  }
}
