/*******************************************************************
 *  An example of bot that echos back any messages received        *
 *                                                                 *
 *  written by Giacarlo Bacchio (Gianbacchio on Github)            *
 *  adapted by Brian Lough                                         *
 *******************************************************************/

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

#include <SPI.h>
#include <SD.h>


// Initialize Wifi connection to the router
char ssid[] = "Mikrotik";              // your network SSID (name)
char password[] = "carolinebrian";                              // your network key



// Initialize Telegram BOT
#define BOTtoken "245123272:AAF0si333T4uO9b8soU7sC1nNy2ksmEdqFU" //token of test2

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

int Bot_mtbs = 1000; //mean time between scan messages
long Bot_lasttime;   //last time messages' scan has been done


void setup() {

  Serial.begin(115200);

  Serial.print("Initializing SD card...");

  if (!SD.begin(D0)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");


  // Set WiFi to station mode and disconnect from an AP if it was Previously
  // connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // Attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);

  //ESP.wdtEnable(15000000); // make the watch dog timeout longer

}

void loop() {

  if (millis() > Bot_lasttime + Bot_mtbs)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while(numNewMessages) {
      Serial.println("got response");

      DynamicJsonBuffer jsonBuffer;
      JsonObject& payload = jsonBuffer.createObject();
      payload["chat_id"] = bot.messages[0].chat_id;
      
      File myFile = SD.open("box.jpg");
      if (myFile) {
        Serial.println("box.jpg:");

        bot.sendImageFromFileToTelegram(&myFile, bot.messages[0].chat_id);
        // close the file:
        myFile.close();
        } else {
          // if the file didn't open, print an error:
          Serial.println("error opening image.jpg");
      }
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    Bot_lasttime = millis();
  }
}
