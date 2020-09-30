/*******************************************************************
    A telegram bot for your ESP32 that demonstrates sending an image
    from SD.

    Parts:
    ESP32 D1 Mini stlye Dev board* - http://s.click.aliexpress.com/e/C6ds4my
    (or any ESP32 board)

      = Affilate

    If you find what I do useful and would like to support me,
    please consider becoming a sponsor on Github
    https://github.com/sponsors/witnessmenow/

    Example originally written by Vadim Sinitski 

    Library written by Brian Lough
    YouTube: https://www.youtube.com/brianlough
    Tindie: https://www.tindie.com/stores/brianlough/
    Twitter: https://twitter.com/witnessmenow
 *******************************************************************/
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <SD.h>

// Wifi network station credentials
#define WIFI_SSID "YOUR_SSID"
#define WIFI_PASSWORD "YOUR_PASSWORD"
// Telegram BOT Token (Get from Botfather)
#define BOT_TOKEN "XXXXXXXXX:XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"

const unsigned long BOT_MTBS = 1000; // mean time between scan messages
#define SD_CS 5

unsigned long bot_lasttime;          // last time messages' scan has been done
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

File myFile;
bool isMoreDataAvailable();
byte getNextByte();

bool isMoreDataAvailable()
{
  return myFile.available();
}

byte getNextByte()
{
  return myFile.read();
}

void handleNewMessages(int numNewMessages)
{
  String chat_id = bot.messages[0].chat_id;
  String file_name = "box.jpg";

  myFile = SD.open(file_name);

  if (myFile)
  {
    Serial.print(file_name);
    Serial.print("....");

    //Content type for PNG image/png
    String sent = bot.sendPhotoByBinary(chat_id, "image/jpeg", myFile.size(),
                                        isMoreDataAvailable,
                                        getNextByte, nullptr, nullptr);

    if (sent)
    {
      Serial.println("was successfully sent");
    }
    else
    {
      Serial.println("was not sent");
    }

    myFile.close();
  }
  else
  {
    // if the file didn't open, print an error:
    Serial.println("error opening photo");
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.println();

  Serial.print("Initializing SD card....");
  if (!SD.begin(SD_CS))
  {
    Serial.println("failed!");
    return;
  }
  Serial.println("done.");

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
