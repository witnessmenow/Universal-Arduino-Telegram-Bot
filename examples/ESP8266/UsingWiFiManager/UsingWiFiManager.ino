/*******************************************************************
 *  An example that uses the WifiManager library for wifi          *
 *  wifi connection. bot that receives commands and turns on and   *
 *  off an LED.                                                    *
 *                                                                 *
 *  written by Brian Lough                                         *
 *******************************************************************/

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

#include <EEPROM.h>

#include <DNSServer.h>            //Local DNS Server used for redirecting all rs to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic

#define BOT_TOKEN_LENGTH 46

const int ledPin = D4;
const int resetConfigPin = D3; //When high will reset the wifi manager config

char botToken[BOT_TOKEN_LENGTH] = "";

WiFiClientSecure client;
UniversalTelegramBot *bot;

int Bot_mtbs = 1000; //mean time between scan messages
long Bot_lasttime;   //last time messages' scan has been done
bool Start = false;
int ledStatus = 0;

//flag for saving data
bool shouldSaveConfig = false;

//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

void readBotTokenFromEeprom(int offset){
  for(int i = offset; i<BOT_TOKEN_LENGTH; i++ ){
    botToken[i] = EEPROM.read(i);
  }
  EEPROM.commit();
}

void writeBotTokenToEeprom(int offset){
  for(int i = offset; i<BOT_TOKEN_LENGTH; i++ ){
    EEPROM.write(i, botToken[i]);
  }
  EEPROM.commit();
}

void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));
  for(int i=0; i<numNewMessages; i++) {
    String chat_id = String(bot->messages[i].chat_id);
    String text = bot->messages[i].text;
    if (text == "/ledon") {
      digitalWrite(ledPin, HIGH);   // turn the LED on (HIGH is the voltage level)
      ledStatus = 1;
      bot->sendMessage(chat_id, "Led is ON", "");
    }
    if (text == "/ledoff") {
      ledStatus = 0;
      digitalWrite(ledPin, LOW);    // turn the LED off (LOW is the voltage level)
      bot->sendMessage(chat_id, "Led is OFF", "");
    }
    if (text == "/status") {
      if(ledStatus){
        bot->sendMessage(chat_id, "Led is ON", "");
      } else {
        bot->sendMessage(chat_id, "Led is OFF", "");
      }
    }
    if (text == "/start") {
      String welcome = "Welcome from FlashLedBot, your personal Bot on ESP8266\n";
      welcome = welcome + "/ledon : to switch the Led ON\n";
      welcome = welcome + "/ledoff : to switch the Led OFF\n";
      welcome = welcome + "/status : Returns current status of LED\n";
      bot->sendMessage(chat_id, welcome, "Markdown");
    }
  }
}


void setup() {
  Serial.begin(115200);

  EEPROM.begin(BOT_TOKEN_LENGTH);
  pinMode(resetConfigPin, INPUT);
  pinMode(ledPin, OUTPUT); // initialize digital ledPin as an output.
  delay(10);
  digitalWrite(ledPin, LOW); // initialize pin as off

  Serial.println("read bot token");
  readBotTokenFromEeprom(0);
  Serial.println(botToken);

  WiFiManager wifiManager;
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  //Adding an additional config on the WIFI manager webpage for the bot token
  WiFiManagerParameter custom_bot_id("botid", "Bot Token", botToken, 50);
  wifiManager.addParameter(&custom_bot_id);
  //If it fails to connect it will create a TELEGRAM-BOT access point
  wifiManager.autoConnect("TELEGRAM-BOT");

  strcpy(botToken, custom_bot_id.getValue());
  if (shouldSaveConfig) {
    writeBotTokenToEeprom(0);
  }

  bot = new UniversalTelegramBot(botToken, client);
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);

}

void loop() {
  if ( digitalRead(resetConfigPin) == LOW ) {
    Serial.println("Reset");
    WiFi.disconnect();
    Serial.println("Dq");
    delay(500);
    ESP.reset();
    delay(5000);
  }
  if (millis() > Bot_lasttime + Bot_mtbs)  {
    int numNewMessages = bot->getUpdates(bot->last_message_received + 1);
    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot->getUpdates(bot->last_message_received + 1);
    }
    Bot_lasttime = millis();
  }
}
