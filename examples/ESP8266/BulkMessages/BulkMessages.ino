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
char ssid[] = "XXXXXX";     // your network SSID (name)
char password[] = "YYYYYY"; // your network key

// Initialize Telegram BOT
#define BOTtoken "XXXXXXXXX:XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"  // your Bot Token (Get from Botfather)

WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOTtoken, secured_client);

int Bot_mtbs = 1000; // mean time between scan messages
long Bot_lasttime;   // last time messages' scan has been done

int bulk_messages_mtbs = 1500; // mean time between send messages, 1.5 seconds
int messages_limit_per_second = 25; // Telegram API have limit for bulk messages ~30 messages per second

String subscribed_users_filename = "subscribed_users.json";

DynamicJsonBuffer jsonBuffer;

void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;

    String from_name = bot.messages[i].from_name;
    if (from_name == "") from_name = "Guest";

    if (text == "/start") {
      if (addSubscribedUser(chat_id, from_name)) {
        String welcome = "Welcome to Universal Arduino Telegram Bot library, " + from_name + ".\n";
        welcome += "This is Bulk Messages example.\n\n";
        welcome += "/showallusers : show all subscribed users\n";
        welcome += "/testbulkmessage : send test message to subscribed users\n";
        welcome += "/removeallusers : remove all subscribed users\n";
        welcome += "/stop : unsubscribe from bot\n";
        bot.sendMessage(chat_id, welcome, "Markdown");
      } else {
        bot.sendMessage(chat_id, "Something wrong, please try again (later?)", "");
      }
    }

    if (text == "/stop") {
      if (removeSubscribedUser(chat_id)) {
        bot.sendMessage(chat_id, "Thank you, " + from_name + ", we always waiting you back", "");
      } else {
        bot.sendMessage(chat_id, "Something wrong, please try again (later?)", "");
      }
    }

    if (text == "/testbulkmessage") {
      sendMessageToAllSubscribedUsers("ATTENTION, this is bulk message for all subscribed users!");
    }

    if (text == "/showallusers") {
      File subscribedUsersFile = SPIFFS.open("/"+subscribed_users_filename, "r");

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

    if (text == "/removeallusers") {
      if (SPIFFS.remove("/"+subscribed_users_filename)) {
        bot.sendMessage(chat_id, "All users removed", "");
      } else {
        bot.sendMessage(chat_id, "Something wrong, please try again (later?)", "");
      }
    }
  }
}

JsonObject& getSubscribedUsers() {
  File subscribedUsersFile = SPIFFS.open("/"+subscribed_users_filename, "r");

  if (!subscribedUsersFile) {
    Serial.println("Failed to open subscribed users file");

    // Create empty file (w+ not working as expect)
    File f = SPIFFS.open("/"+subscribed_users_filename, "w");
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

    return users;
  }
}

bool addSubscribedUser(String chat_id, String from_name) {
  JsonObject& users = getSubscribedUsers();

  File subscribedUsersFile = SPIFFS.open("/"+subscribed_users_filename, "w+");

  if (!subscribedUsersFile) {
    Serial.println("Failed to open subscribed users file for writing");
    //return false;
  }

  users.set(chat_id, from_name);
  users.printTo(subscribedUsersFile);

  subscribedUsersFile.close();

  return true;
}

bool removeSubscribedUser(String chat_id) {
  JsonObject& users = getSubscribedUsers();

  File subscribedUsersFile = SPIFFS.open("/"+subscribed_users_filename, "w");

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

  JsonObject& users = getSubscribedUsers();

  for (JsonObject::iterator it=users.begin(); it!=users.end(); ++it) {
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
  Serial.begin(115200);

  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount file system");
    return;
  }

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
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    Bot_lasttime = millis();
  }
}
