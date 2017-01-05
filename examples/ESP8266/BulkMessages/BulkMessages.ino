/*******************************************************************
 *  An example of how to use a bulk messages to subscribed users.  *
 *                                                                 *
 *                                                                 *
 *  written by Vadim Sinitski                                      *
 *******************************************************************/
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <FS.h>
#include <ArduinoJson.h>

// Initialize Wifi connection to the router
char ssid[] = "xxxxxxx";        // your network SSID (name)
char password[] = "yyyyyyyy"; // your network key

// Initialize Telegram BOT
#define BOTtoken "XXXXXXXXX:XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"  // your Bot Token (Get from Botfather)

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

int Bot_mtbs = 1000; //mean time between scan messages
long Bot_lasttime;   //last time messages' scan has been done

int bulk_messages_mtbs = 1500; //mean time between send messages, 1.5 seconds
int messages_limit_per_second = 25; // Telegram API have limit for bulk messages ~30 messages in 1 second

void handleNewMessages(int numNewMessages) {
    Serial.println("handleNewMessages");
    Serial.println(String(numNewMessages));

    for(int i=0; i<numNewMessages; i++) {
        String chat_id = String(bot.messages[i].chat_id);
        String text = bot.messages[i].text;

        if (text == "/start") {
            if (addSubscribedUser(chat_id)) {
              bot.sendMessage(chat_id, "Welcome!", "");
            } else {
              bot.sendMessage(chat_id, "Something wrong, please try again", "");
            }
        }

        if (text == "/stop") {
            bot.sendMessage(chat_id, "Thank you, we always waiting you back", "");
            removeSubscribedUser(chat_id);
        }

        if (text == "/testbulkmessage") {
            sendMessageToAllSubscribedUsers("Current temperature is 0.0C");
        }

        if (text == "\/showallusers") {
          File subscribedUsersFile = SPIFFS.open("/subscribed_users.json", "r");

          if (!subscribedUsersFile) {
            bot.sendMessage(chat_id, "No subscription file", "");
          }

          size_t size = subscribedUsersFile.size();

          if (size > 1024) {
              bot.sendMessage(chat_id, "Subscribed users file is too large", "");
          } else {
              String file_content = subscribedUsersFile.readString();
              bot.sendMessage(chat_id, file_content, "");
          }
        }

       if (text == "\/removeallusers") {
          SPIFFS.remove("/subscribed_users.json");
        }
    }
}

JsonObject& getSubscribedUsers(JsonBuffer& jsonBuffer) {
    File subscribedUsersFile = SPIFFS.open("/subscribed_users.json", "r");

    if (!subscribedUsersFile) {

        Serial.println("Failed to open subscribed users file");

        // Create empyt file (w+ not working as expect)
        File f = SPIFFS.open("/subscribed_users.json", "w");
        f.close();

        JsonObject& users = jsonBuffer.createObject();

        return users;
    } else {

      size_t size = subscribedUsersFile.size();

      if (size > 1024) {
          Serial.println("Subscribed users file is too large");
          //return users;
      }

      String file_content = subscribedUsersFile.readString();

      JsonObject& users = jsonBuffer.parseObject(file_content);

      if (!users.success()) {
          Serial.println("Failed to parse subscribed users file");
          return users;
      }

      subscribedUsersFile.close();

  //    Serial.println("Test");
  //    users.printTo(Serial);

      return users;
    }

}

bool addSubscribedUser(String chat_id) {
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& users = getSubscribedUsers(jsonBuffer);

    File subscribedUsersFile = SPIFFS.open("/subscribed_users.json", "w+");

    if (!subscribedUsersFile) {
        Serial.println("Failed to open subscribed users file for writing");
       //return false;
    }

    users.set(chat_id, 1);
    users.printTo(subscribedUsersFile);

    subscribedUsersFile.close();

    return true;
}

bool removeSubscribedUser(String chat_id) {
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& users = getSubscribedUsers(jsonBuffer);

    File subscribedUsersFile = SPIFFS.open("/subscribed_users.json", "w");

    if (!subscribedUsersFile) {
        Serial.println("Failed to open subscribed users file for writing");
        return false;
    }

    users.remove(chat_id);
    users.printTo(subscribedUsersFile);

    subscribedUsersFile.close();

    return true;
}

void sendMessageToAllSubscribedUsers(String message) {
  int users_processed = 0;

  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& users = getSubscribedUsers(jsonBuffer);

  for(JsonObject::iterator it=users.begin(); it!=users.end(); ++it)
  {
    users_processed++;

    if (users_processed < messages_limit_per_second)  {
        const char* chat_id = it->key;
        bot.sendMessage(chat_id, message, "");
    } else {
        delay(bulk_messages_mtbs);
        users_processed = 0;
    }
  }
}

void setup() {
    Serial.begin(9600);

    if (!SPIFFS.begin()) {
        Serial.println("Failed to mount file system");
        return;
    }

    // Set WiFi to station mode and disconnect from an AP if it was Previously
    // connected
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
    Serial.println("IP address: ");
    IPAddress ip = WiFi.localIP();
    Serial.println(ip);
}

void loop() {
    if (millis() > Bot_lasttime + Bot_mtbs)  {
        int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

        while(numNewMessages) {
            Serial.println("got response");
            handleNewMessages(numNewMessages);
            numNewMessages = bot.getUpdates(bot.last_message_received + 1);
        }

        Bot_lasttime = millis();
    }
}