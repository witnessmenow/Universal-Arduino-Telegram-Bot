/*******************************************************************
 *  An example of how to use a bulk messages to subscribed users.  *
 *                                                                 *
 *  written by Vadim Sinitski                                      *
 *******************************************************************/
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <LittleFS.h>

// Wifi network station credentials
#define WIFI_SSID "YOUR_SSID"
#define WIFI_PASSWORD "YOUR_PASSWORD"
// Telegram BOT Token (Get from Botfather)
#define BOT_TOKEN "XXXXXXXXX:XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"

const char *SUBSCRIBED_USERS_FILENAME = "/subscribed_users.json"; // Filename for local storage
const unsigned long BULK_MESSAGES_MTBS = 1500;                    // Mean time between send messages, 1.5 seconds
const unsigned int MESSAGES_LIMIT_PER_SECOND = 25;                // Telegram API have limit for bulk messages ~30 messages per second
const unsigned long BOT_MTBS = 1000;                              // Mean time between scan messages

WiFiClientSecure secured_client;
X509List cert(TELEGRAM_CERTIFICATE_ROOT);
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
DynamicJsonDocument usersDoc(1500);
unsigned long bot_lasttime; // last time messages' scan has been done

JsonObject getSubscribedUsers()
{
  File subscribedUsersFile = LittleFS.open(SUBSCRIBED_USERS_FILENAME, "r");
  JsonObject users;

  // no file
  if (!subscribedUsersFile)
  {
    Serial.println("Failed to open subscribed users file");
    // Create empty file (w+ not working as expect)
    File f = LittleFS.open(SUBSCRIBED_USERS_FILENAME, "w");
    f.close();
    return users;
  }

  // too large file
  size_t size = subscribedUsersFile.size();
  if (size > 1500)
  {
    subscribedUsersFile.close();
    Serial.println("Subscribed users file is too large");
    return users;
  }

  String file_content = subscribedUsersFile.readString();
  subscribedUsersFile.close();
  DeserializationError error = deserializeJson(usersDoc, file_content);
  if (error)
  {
    Serial.println("Failed to parse subscribed users file");
    return users;
  }
  users = usersDoc.as<JsonObject>();

  return users;
}

bool addSubscribedUser(String chat_id, String from_name)
{
  JsonObject users = getSubscribedUsers();
  users[chat_id] = from_name;

  File subscribedUsersFile = LittleFS.open(SUBSCRIBED_USERS_FILENAME, "w+");
  // file not available
  if (!subscribedUsersFile)
  {
    subscribedUsersFile.close();
    Serial.println("Failed to open subscribed users file for writing");
    return false;
  }

  //users.getOrCreateMember(chat_id, from_name);
  serializeJson(users, subscribedUsersFile);
  subscribedUsersFile.close();
  return true;
}

bool removeSubscribedUser(String chat_id)
{
  JsonObject users = getSubscribedUsers();
  users.remove(chat_id);

  File subscribedUsersFile = LittleFS.open(SUBSCRIBED_USERS_FILENAME, "w");
  // file not available
  if (!subscribedUsersFile)
  {
    subscribedUsersFile.close();
    Serial.println("Failed to open subscribed users file for writing");
    return false;
  }

  serializeJson(users, subscribedUsersFile);
  subscribedUsersFile.close();
  return true;
}

void sendMessageToAllSubscribedUsers(String message)
{
  JsonObject users = getSubscribedUsers();
  unsigned int users_processed = 0;

  for (JsonObject::iterator it = users.begin(); it != users.end(); ++it)
  {
    users_processed++;
    if (users_processed < MESSAGES_LIMIT_PER_SECOND)
    {
      const char *chat_id = it->key().c_str();
      bot.sendMessage(chat_id, message, "");
    }
    else
    {
      delay(BULK_MESSAGES_MTBS);
      users_processed = 0;
    }
  }
}

void handleNewMessages(int numNewMessages)
{
  Serial.print("handleNewMessages: ");
  Serial.println(numNewMessages);

  for (int i = 0; i < numNewMessages; i++)
  {
    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;
    String from_name = bot.messages[i].from_name;
    if (from_name == "")
      from_name = "Guest";

    if (text == "/start")
    {
      if (addSubscribedUser(chat_id, from_name))
      {
        String welcome = "Welcome to Universal Arduino Telegram Bot library.\n";
        welcome += "This is Bulk Messages example.\n\n";
        welcome += "You, " + from_name + ", have been subscribed.\n";
        welcome += "/showallusers : show all subscribed users\n";
        welcome += "/testbulkmessage : send test message to subscribed users\n";
        welcome += "/removeallusers : remove all subscribed users\n";
        welcome += "/stop : unsubscribe from bot\n";
        bot.sendMessage(chat_id, welcome, "Markdown");
      }
      else
      {
        bot.sendMessage(chat_id, "Something wrong, please try again (later?)", "");
      }
    }

    if (text == "/stop")
    {
      if (removeSubscribedUser(chat_id))
      {
        bot.sendMessage(chat_id, "Thank you, " + from_name + ", we always waiting you back", "");
      }
      else
      {
        bot.sendMessage(chat_id, "Something wrong, please try again (later?)", "");
      }
    }

    if (text == "/testbulkmessage")
    {
      sendMessageToAllSubscribedUsers("ATTENTION, this is bulk message for all subscribed users!");
    }

    if (text == "/showallusers")
    {
      File subscribedUsersFile = LittleFS.open(SUBSCRIBED_USERS_FILENAME, "r");
      // no file
      if (!subscribedUsersFile)
      {
        bot.sendMessage(chat_id, "No subscription file", "");
        return;
      }
      size_t size = subscribedUsersFile.size();
      if (size > 1024)
      {
        bot.sendMessage(chat_id, "Subscribed users file is too large", "");
      }
      else
      {
        String file_content = subscribedUsersFile.readString();
        bot.sendMessage(chat_id, file_content, "");
      }
    }

    if (text == "/removeallusers")
    {
      if (LittleFS.remove(SUBSCRIBED_USERS_FILENAME))
      {
        bot.sendMessage(chat_id, "All users removed", "");
      }
      else
      {
        bot.sendMessage(chat_id, "Something wrong, please try again (later?)", "");
      }
    }
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.println();

  if (!LittleFS.begin())
  {
    Serial.println("Failed to mount file system");
    return;
  }

  // attempt to connect to Wifi network:
  configTime(0, 0, "pool.ntp.org");      // get UTC time via NTP
  secured_client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org
  Serial.print("Connecting to Wifi SSID ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());

  // Check NTP/Time, usually it is instantaneous and you can delete the code below.
  Serial.print("Retrieving time: ");
  time_t now = time(nullptr);
  while (now < 24 * 3600)
  {
    Serial.print(".");
    delay(100);
    now = time(nullptr);
  }
  Serial.println(now);
}

void loop()
{
  if (millis() - bot_lasttime > BOT_MTBS)
  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages)
    {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    bot_lasttime = millis();
  }
}
