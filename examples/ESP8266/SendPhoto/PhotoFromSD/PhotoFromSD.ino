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
char ssid[] = "XXXXXX";     // your network SSID (name)
char password[] = "YYYYYY"; // your network key

// Initialize Telegram BOT
#define BOTtoken "XXXXXXXXX:XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"  // your Bot Token (Get from Botfather)

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

int Bot_mtbs = 1000; //mean time between scan messages
long Bot_lasttime;   //last time messages' scan has been done

File myFile;
bool isMoreDataAvailable();
byte getNextByte();

void setup() {
  Serial.begin(115200);

  Serial.print("Initializing SD card....");

  if (!SD.begin(D0)) {
    Serial.println("failed!");
    return;
  }

  Serial.println("done.");

  // Set WiFi to station mode and disconnect from an AP if it was Previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if (millis() > Bot_lasttime + Bot_mtbs)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      Serial.println("got response");

      String chat_id = bot.messages[0].chat_id;
      String file_name = "box.jpg";

      myFile = SD.open(file_name);

      if (myFile) {
        Serial.print(file_name);
        Serial.print("....");

        //Content type for PNG image/png
        String sent = bot.sendPhotoByBinary(chat_id, "image/jpeg", myFile.size(),
            isMoreDataAvailable,
            getNextByte);

        if (sent) {
          Serial.println("was successfully sent");
        } else {
          Serial.println("was not sent");
        }

        myFile.close();
      } else {
        // if the file didn't open, print an error:
        Serial.println("error opening photo");
      }

      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    Bot_lasttime = millis();
  }
}

bool isMoreDataAvailable(){
  return myFile.available();
}

byte getNextByte(){
  return myFile.read();
}
