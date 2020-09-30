/*******************************************************************
    An example of how to use a custom reply keyboard markup.


     written by Vadim Sinitski (modified by Brian Lough)
 *******************************************************************/
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

// Wifi network station credentials
#define WIFI_SSID "YOUR_SSID"
#define WIFI_PASSWORD "YOUR_PASSWORD"
// Telegram BOT Token (Get from Botfather)
#define BOT_TOKEN "XXXXXXXXX:XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"

const unsigned long BOT_MTBS = 1000; // mean time between scan messages

WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
unsigned long bot_lasttime;          // last time messages' scan has been done

void handleNewMessages(int numNewMessages)
{
  for (int i = 0; i < numNewMessages; i++)
  {
    // Inline buttons with callbacks when pressed will raise a callback_query message
    if (bot.messages[i].type == "callback_query")
    {
      Serial.print("Call back button pressed by: ");
      Serial.println(bot.messages[i].from_id);
      Serial.print("Data on the button: ");
      Serial.println(bot.messages[i].text);
      bot.sendMessage(bot.messages[i].from_id, bot.messages[i].text, "");
    }
    else
    {
      String chat_id = bot.messages[i].chat_id;
      String text = bot.messages[i].text;

      String from_name = bot.messages[i].from_name;
      if (from_name == "")
        from_name = "Guest";

      if (text == "/options")
      {
        String keyboardJson = "[[{ \"text\" : \"Go to Google\", \"url\" : \"https://www.google.com\" }],[{ \"text\" : \"Send\", \"callback_data\" : \"This was sent by inline\" }]]";
        bot.sendMessageWithInlineKeyboard(chat_id, "Choose from one of the following options", "", keyboardJson);
      }

      if (text == "/start")
      {
        String welcome = "Welcome to Universal Arduino Telegram Bot library, " + from_name + ".\n";
        welcome += "This is Inline Keyboard Markup example.\n\n";
        welcome += "/options : returns the inline keyboard\n";

        bot.sendMessage(chat_id, welcome, "Markdown");
      }
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
