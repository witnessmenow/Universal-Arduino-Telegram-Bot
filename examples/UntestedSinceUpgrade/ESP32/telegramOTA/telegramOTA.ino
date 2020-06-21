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


// Initialize Wifi connection to the router
char ssid[] = "SSID";     // your network SSID (name)
char password[] = "password"; // your network key

// Initialize Telegram BOT
#define BOTtoken "XXXXXXXXX:XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"  // your Bot Token (Get from Botfather)

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

int Bot_mtbs = 1000; //mean time between scan messages
long Bot_lasttime;   //last time messages' scan has been done

void setup() {
  Serial.begin(115200);
  if (!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
  }
  // Attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);

  // Set WiFi to station mode and disconnect from an AP if it was Previously
  // connected
  WiFi.mode(WIFI_STA);
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

    while (numNewMessages) {
      Serial.println("got response");
      for (int i = 0; i < numNewMessages; i++) {
        if (bot.messages[i].type == "message") {
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
                if (http.begin(client, bot.messages[i].file_path))
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
                ret = httpUpdate.update(client, bot.messages[i].file_path);
              }
              if (bot.messages[i].file_caption == "update spiffs")
              {
                bot.sendMessage(bot.messages[i].chat_id, "SPIFFS writing...", "");
                ret = httpUpdate.updateSpiffs(client, bot.messages[i].file_path);
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
          } else if (bot.messages[i].text == "/format")
          {
            bool res = SPIFFS.format();
            if (!res)
              bot.sendMessage(bot.messages[i].chat_id, "Format unsuccessful", "");
            else
              bot.sendMessage(bot.messages[i].chat_id, "SPIFFS formatted.", "");
          }
        }
      }
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    Bot_lasttime = millis();
  }
}
