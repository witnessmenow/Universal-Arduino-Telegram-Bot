/*******************************************************************
   An example of bot that can update firmware or spiffs and write file to spiffs.

   written by Selim Olcer
*******************************************************************/
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <HTTPUpdate.h>
#include <SD.h>
#include <FS.h>
#include <SPIFFS.h>

// Wifi network station credentials
#define WIFI_SSID "YOUR_SSID"
#define WIFI_PASSWORD "YOUR_PASSWORD"
// Telegram BOT Token (Get from Botfather)
#define BOT_TOKEN "XXXXXXXXX:XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"

const unsigned long BOT_MTBS = 1000; // mean time between scan messages

WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
unsigned long bot_lasttime; // last time messages' scan has been done

void handleNewMessages(int numNewMessages)
{
  for (int i = 0; i < numNewMessages; i++)
  {
    if (bot.messages[i].type == "message")
    {
      if (bot.messages[i].hasDocument == true)
      {
        httpUpdate.rebootOnUpdate(false);
        t_httpUpdate_return ret = (t_httpUpdate_return)3;
        if (bot.messages[i].file_caption == "write spiffs")
        {
          size_t spiffsFreeSize = SPIFFS.totalBytes() - SPIFFS.usedBytes();
          if (bot.messages[i].file_size < spiffsFreeSize)
          {
            bot.sendMessage(bot.messages[i].chat_id, "File downloading.", "");
            HTTPClient http;
            if (http.begin(secured_client, bot.messages[i].file_path))
            {
              int code = http.GET();
              if (code == HTTP_CODE_OK)
              {
                int total = http.getSize();
                int len = total;
                uint8_t buff[128] = {0};
                WiFiClient *tcp = http.getStreamPtr();
                if (SPIFFS.exists("/" + bot.messages[i].file_name))
                  SPIFFS.remove(("/" + bot.messages[i].file_name));
                File fl = SPIFFS.open("/" + bot.messages[i].file_name, FILE_WRITE);
                if (!fl)
                {
                  bot.sendMessage(bot.messages[i].chat_id, "File open error.", "");
                }
                else
                {
                  while (http.connected() && (len > 0 || len == -1))
                  {
                    size_t size_available = tcp->available();
                    Serial.print("%");
                    Serial.println(100 - ((len * 100) / total));
                    if (size_available)
                    {
                      int c = tcp->readBytes(buff, ((size_available > sizeof(buff)) ? sizeof(buff) : size_available));
                      fl.write(buff, c);
                      if (len > 0)
                      {
                        len -= c;
                      }
                    }
                    delay(1);
                  }
                  fl.close();
                  if (len == 0)
                    bot.sendMessage(bot.messages[i].chat_id, "Success.", "");
                  else
                    bot.sendMessage(bot.messages[i].chat_id, "Error.", "");
                }
              }
              http.end();
            }
          }
          else
          {
            bot.sendMessage(bot.messages[i].chat_id, "SPIFFS size to low (" + String(spiffsFreeSize) + ") needed: " + String(bot.messages[i].file_size), "");
          }
        }
        else
        {
          if (bot.messages[i].file_caption == "update firmware")
          {
            bot.sendMessage(bot.messages[i].chat_id, "Firmware writing...", "");
            ret = httpUpdate.update(secured_client, bot.messages[i].file_path);
          }
          if (bot.messages[i].file_caption == "update spiffs")
          {
            bot.sendMessage(bot.messages[i].chat_id, "SPIFFS writing...", "");
            ret = httpUpdate.updateSpiffs(secured_client, bot.messages[i].file_path);
          }
          switch (ret)
          {
          case HTTP_UPDATE_FAILED:
            bot.sendMessage(bot.messages[i].chat_id, "HTTP_UPDATE_FAILED Error (" + String(httpUpdate.getLastError()) + "): " + httpUpdate.getLastErrorString(), "");
            break;

          case HTTP_UPDATE_NO_UPDATES:
            bot.sendMessage(bot.messages[i].chat_id, "HTTP_UPDATE_NO_UPDATES", "");
            break;

          case HTTP_UPDATE_OK:
            bot.sendMessage(bot.messages[i].chat_id, "UPDATE OK.\nRestarting...", "");
            numNewMessages = bot.getUpdates(bot.last_message_received + 1);
            ESP.restart();
            break;
          default:
            break;
          }
        }
      }
      if (bot.messages[i].text == "/dir")
      {
        File root = SPIFFS.open("/");
        File file = root.openNextFile();
        String files = "";
        while (file)
        {
          files += String(file.name()) + " " + String(file.size()) + "B\n";
          file = root.openNextFile();
        }
        bot.sendMessage(bot.messages[i].chat_id, files, "");
      }
      else if (bot.messages[i].text == "/format")
      {
        bool res = SPIFFS.format();
        if (!res)
          bot.sendMessage(bot.messages[i].chat_id, "Format unsuccessful", "");
        else
          bot.sendMessage(bot.messages[i].chat_id, "SPIFFS formatted.", "");
      }
    }
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.println();

  if (!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
  }

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
