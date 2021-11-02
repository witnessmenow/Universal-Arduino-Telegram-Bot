#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

// Wifi network station credentials
#define WIFI_SSID "YOUR_SSID"
#define WIFI_PASSWORD "YOUR_PASSWORD"
// Telegram BOT Token (Get from Botfather)
#define BOT_TOKEN "XXXXXXXXX:XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"

const unsigned long BOT_MTBS = 1000; // mean time between scan messages

X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client, 2500);
unsigned long bot_lasttime;

void setup()
{
  Serial.begin(115200);

  connectToWifi();
  synchronizeTime();
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

void handleNewMessages(int numNewMessages)
{
  for (int i = 0; i < numNewMessages; i++)
  {
    telegramMessage message = bot.messages[i];

    Serial.println("Received: " + message.text);

    String text = message.text;
    String from_name = message.from_name;
    int message_id = getMessageId(message);

    if (from_name == "")
    {
      from_name = "Guest";
    }

    if (text == "/options")
    {
      // Memory pool for JSON object tree.
      //
      // Inside the brackets, 400 is the size of the pool in bytes.
      // Don't forget to change this value to match your JSON document.
      // Use https://arduinojson.org/assistant to compute the capacity.
      StaticJsonDocument<400> keyboard;

      StaticJsonDocument<100> menuItem0;
      menuItem0["text"] = "Some button";
      menuItem0["callback_data"] = "/custom_action";
      keyboard[0].add(menuItem0);

      StaticJsonDocument<100> menuItem1;
      menuItem1["text"] = "Go to Google";
      menuItem1["url"] = "https://www.google.com";
      keyboard[1].add(menuItem1);

      StaticJsonDocument<100> menuItem2;
      menuItem2["text"] = "Close";
      menuItem2["callback_data"] = "/cancel";
      keyboard[2].add(menuItem2);

      bot.sendMessageWithInlineKeyboard(
        message.chat_id,
        "Choose from one of the following options",
        "",
        getKeyboardJson(keyboard),
        message_id
      );
    }
    else if (text == "/custom_action")
    {
      Serial.println(
        message_id != 0 ? "The keyboard will be replaced with test text." : "Only test text will be displayed."
      );

      bot.sendMessage(message.chat_id, "Lorem Ipsum", "", message_id);
    }
    else if (text == "/cancel")
    {
      Serial.println(
        message_id != 0 ? "Keyboard will be deleted." : "Action will be ignored."
      );

      bot.deleteMessage(message.chat_id, message_id);
    }
    else if (text == "/start")
    {
      String welcome = "Welcome to Universal Arduino Telegram Bot library, " + from_name + ".\n";
      welcome += "This is example of Inline Keyboard Markup and deletion of the previous message.\n\n";
      welcome += "/options : returns the inline keyboard\n";
      welcome += "/custom_action : returns test text\n";

      bot.sendMessage(message.chat_id, welcome, "");
    }
  }
}

void connectToWifi()
{
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  Serial.print("Connecting to Wifi SSID ");
  Serial.print(WIFI_SSID);
  Serial.print(" ");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  secured_client.setTrustAnchors(&cert);

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(1000);
  }
  Serial.println();

  Serial.print("WiFi connected. IP address: ");
  Serial.println(WiFi.localIP());
}

void synchronizeTime()
{
  configTime(0, 0, "pool.ntp.org");

  Serial.print("Time synchronization ");

  time_t now = time(nullptr);
  while (now < 24 * 3600)
  {
    Serial.print(".");
    delay(100);
    now = time(nullptr);
  }
  Serial.println();

  Serial.print("Current time (unixtime): ");
  Serial.println(now);
}

int getMessageId(telegramMessage message)
{
  if (message.type == "callback_query")
  {
    Serial.println("callback_query detected. The message_id will be passed.");
  }

  return message.type == "callback_query" ? message.message_id : 0;
}

String getKeyboardJson(JsonDocument& keyboard)
{
  String keyboardJson = "";
  serializeJson(keyboard, keyboardJson);
  return keyboardJson;
}