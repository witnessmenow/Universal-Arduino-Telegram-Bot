/*******************************************************************
 *  An example of recieving location Data
 *
 *
 *  By Brian Lough
 *******************************************************************/
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

const unsigned long BOT_MTBS = 1000; // mean time between scan messages

// Wifi network station credentials
#define WIFI_SSID "YOUR_SSID"
#define WIFI_PASSWORD "YOUR_PASSWORD"
// Telegram BOT Token (Get from Botfather)
#define BOT_TOKEN "XXXXXXXXX:XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"

unsigned long bot_lasttime;          // last time messages' scan has been done
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

void handleNewMessages(int numNewMessages)
{
  for (int i = 0; i < numNewMessages; i++)
  {
    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;

    String from_name = bot.messages[i].from_name;
    if (from_name == "")
      from_name = "Guest";

    if (bot.messages[i].longitude != 0 || bot.messages[i].latitude != 0)
    {
      Serial.print("Long: ");
      Serial.println(String(bot.messages[i].longitude, 6));
      Serial.print("Lat: ");
      Serial.println(String(bot.messages[i].latitude, 6));

      String message = "Long: " + String(bot.messages[i].longitude, 6) + "\n";
      message += "Lat: " + String(bot.messages[i].latitude, 6) + "\n";
      bot.sendMessage(chat_id, message, "Markdown");
    }
    else if (text == "/start")
    {
      String welcome = "Welcome to Universal Arduino Telegram Bot library, " + from_name + ".\n";
      welcome += "Share a location or a live location and the bot will respond with the co-ords\n";

      bot.sendMessage(chat_id, welcome, "Markdown");
    }
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.println();

  // attempt to connect to Wifi network:
  Serial.print("Connecting to Wifi SSID ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());

  Serial.print("Retrieving time: ");
  configTime(0, 0, "pool.ntp.org"); // get UTC time via NTP
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

