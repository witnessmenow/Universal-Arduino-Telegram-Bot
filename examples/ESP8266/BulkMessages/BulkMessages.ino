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
#define BOTtoken "XXXXXXXXX:XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"  // your Bot Token (Get off Botfather)

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

int Bot_mtbs = 1000; //mean time between scan messages
long Bot_lasttime;   //last time messages' scan has been done

int bulk_messages_mtbs = 1500; //mean time between send messages, 1.5 seconds
int messages_limit_per_second = 25; // Telegram API have limit for bulk messages ~30 messages in 1 second

StaticJsonBuffer<200> jsonBuffer;
JsonObject& subscribed_users = jsonBuffer.createObject();

void handleNewMessages(int numNewMessages) {
    Serial.println("handleNewMessages");
    Serial.println(String(numNewMessages));

    for(int i=0; i<numNewMessages; i++) {
        String chat_id = String(bot.messages[i].chat_id);
        String text = bot.messages[i].text;

        if (text == "/start") {
            addSubscribedUser(chat_id);
            bot.sendMessage(chat_id, "Welcome", "");
            loadSubscribedUsersFile();
        }

        if (text == "/stop") {
            bot.sendMessage(chat_id, "Thank you, we always waiting you back", "");
            removeSubscribedUser(chat_id);
            loadSubscribedUsersFile();
        }

        if (text == "/testbulkmessage") {
            sendMessageToAllSubscribedUsers("Current temperature is 0.0C");
        }
    }
}

bool loadSubscribedUsersFile() {
    File subscribedUsersFile = SPIFFS.open("/subscribed_users.json", "r");

    if (!subscribedUsersFile) {
        Serial.println("Failed to open subscribed users file");
        return false;
    }

    size_t size = subscribedUsersFile.size();

    if (size > 1024) {
        Serial.println("Subscribed users file is too large");
        return false;
    }

    // Allocate a buffer to store contents of the file.
    std::unique_ptr<char[]> buf(new char[size]);

    // We don't use String here because ArduinoJson library requires the input
    // buffer to be mutable. If you don't use ArduinoJson, you may as well
    // use configFile.readString instead.
    subscribedUsersFile.readBytes(buf.get(), size);

    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& subscribed_users = jsonBuffer.parseObject(buf.get());

    if (!subscribed_users.success()) {
        Serial.println("Failed to parse subscribed users file");
        return false;
    }

    subscribedUsersFile.close();

    subscribed_users.printTo(Serial);

    return true;
}

bool addSubscribedUser(String chat_id) {
    File subscribedUsersFile = SPIFFS.open("/subscribed_users.json", "w");

    if (!subscribedUsersFile) {
        Serial.println("Failed to open subscribed users file for writing");
        return false;
    }

    subscribed_users[chat_id] = 1;

    subscribed_users.printTo(subscribedUsersFile);

    subscribedUsersFile.close();

    return true;
}

bool removeSubscribedUser(String chat_id) {
    File subscribedUsersFile = SPIFFS.open("/subscribed_users.json", "w");

    if (!subscribedUsersFile) {
        Serial.println("Failed to open subscribed users file for writing");
        return false;
    }

    subscribed_users.remove(chat_id);

    subscribed_users.printTo(subscribedUsersFile);

    subscribedUsersFile.close();

    return true;
}

void sendMessageToAllSubscribedUsers(String message) {
    int users_processed = 0;

  for(JsonObject::iterator it=subscribed_users.begin(); it!=subscribed_users.end(); ++it)
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

    loadSubscribedUsersFile();

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
        int numNewMessages = bot.getUpdates(bot.last_message_recived + 1);

        while(numNewMessages) {
            Serial.println("got response");
            handleNewMessages(numNewMessages);
            numNewMessages = bot.getUpdates(bot.last_message_recived + 1);
        }

        Bot_lasttime = millis();
    }
}