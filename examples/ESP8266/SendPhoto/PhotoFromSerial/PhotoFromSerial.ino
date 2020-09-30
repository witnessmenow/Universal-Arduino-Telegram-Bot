#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

// Wifi network station credentials
#define WIFI_SSID "YOUR_SSID"
#define WIFI_PASSWORD "YOUR_PASSWORD"
// Telegram BOT Token (Get from Botfather)
#define BOT_TOKEN "XXXXXXXXX:XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"

const unsigned long BOT_MTBS = 1000; // mean time between scan messages

unsigned long bot_lasttime;          // last time messages' scan has been done
X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

String esp_id;

// Initialize Camera settings
#define PIC_PKT_LEN 128 //data length of each read, dont set this too big because ram is limited
#define PIC_FMT_VGA 7
#define PIC_FMT_CIF 5
#define PIC_FMT_OCIF 3
#define CAM_ADDR 0
#define CAM_SERIAL Serial
#define PIC_FMT PIC_FMT_VGA

const byte cameraAddr = (CAM_ADDR << 5); // addr
int picTotalLen = 0;                     // picture length
unsigned int pktCnt;
unsigned int pktCnt_i = 0;
char pkt[PIC_PKT_LEN];
uint16_t cnt;
int n = 0;

bool isMoreDataAvailable()
{
  return (pktCnt_i < pktCnt);
}

byte *getNextBuffer()
{
  char cmd[] = {0xaa, 0x0e | cameraAddr, 0x00, 0x00, (char)(pktCnt_i & 0xff), (char)((pktCnt_i >> 8) & 0xff)};
  //            cmd[0] = 0xaa;
  //            cmd[1] = 0x0e | cameraAddr;
  //            cmd[2] = 0x00;
  //            cmd[3] = 0x00;
  //            cmd[4] = pktCnt_i & 0xff;
  //            cmd[5] = (pktCnt_i >> 8) & 0xff;
  int retry_cnt = 0;
retry:
  delay(10);
  clearRxBuf();
  sendCmd(cmd, 6);
  cnt = Serial.readBytes((char *)pkt, PIC_PKT_LEN);
  unsigned char sum = 0;
  for (int y = 0; y < cnt - 2; y++)
    sum += pkt[y];
  if (sum != pkt[cnt - 2])
    if (++retry_cnt < 100)
      goto retry;
  pktCnt_i++;

  return (uint8_t *)&pkt[4];
}

int getNextBufferLen()
{
  return cnt - 6;
}

void sendSnapshot(String chat_id)
{ // reading picture from camera serial and sending it directly to telegram
  char cmd[] = {0xaa, 0x0e | cameraAddr, 0x00, 0x00, 0x00, 0x00};
  Serial.println(F("\r\nbegin to take picture"));
  if (n == 0)
    preCapture();
  Capture();
  pktCnt_i = 0;
  pktCnt = (picTotalLen) / (PIC_PKT_LEN - 6); // cutting image into PIC_PKT_LEN chunks
  if ((picTotalLen % (PIC_PKT_LEN - 6)) != 0)
    pktCnt += 1;
  Serial.setTimeout(1000);
  String sent = bot.sendPhotoByBinary(chat_id, "image/jpeg", picTotalLen,
                                      isMoreDataAvailable, nullptr,
                                      getNextBuffer, getNextBufferLen);
  cmd[4] = 0xf0;
  cmd[5] = 0xf0;
  sendCmd(cmd, 6);
  if (sent)
    Serial.println(F("\r\nwas successfully sent"));
  else
    Serial.println(F("\r\nwas not sent"));
  n++;
}

void clearRxBuf()
{
  while (Serial.available())
    Serial.read();
}

void sendCmd(char scmd[], int cmd_len)
{
  for (int i = 0; i < cmd_len; i++)
    Serial.write(scmd[i]);
}

void initializeCam()
{
  char cmd[] = {0xaa, 0x0d | cameraAddr, 0x00, 0x00, 0x00, 0x00};
  unsigned char resp[6];

  Serial.setTimeout(500);
  while (1)
  {
    clearRxBuf();
    sendCmd(cmd, 6);
    Serial.setTimeout(1000);
    if (Serial.readBytes((char *)resp, 6) != 6)
      continue;
    if (resp[0] == 0xaa && resp[1] == (0x0e | cameraAddr) && resp[2] == 0x0d && resp[4] == 0 && resp[5] == 0)
    {
      if (Serial.readBytes((char *)resp, 6) != 6)
        continue;
      if (resp[0] == 0xaa && resp[1] == (0x0d | cameraAddr) && resp[2] == 0 && resp[3] == 0 && resp[4] == 0 && resp[5] == 0)
        break;
    }
  }
  cmd[1] = 0x0e | cameraAddr;
  cmd[2] = 0x0d;
  sendCmd(cmd, 6);
  Serial.println(F("\nCamera initialization done."));
}

void preCapture()
{
  char cmd[] = {0xaa, 0x01 | cameraAddr, 0x00, 0x07, 0x00, PIC_FMT};
  unsigned char resp[6];

  Serial.setTimeout(100);
  while (1)
  {
    clearRxBuf();
    sendCmd(cmd, 6);
    if (Serial.readBytes((char *)resp, 6) != 6)
      continue;
    if (resp[0] == 0xaa && resp[1] == (0x0e | cameraAddr) && resp[2] == 0x01 && resp[4] == 0 && resp[5] == 0)
      break;
  }
}

void Capture()
{
  char cmd[] = {0xaa, 0x06 | cameraAddr, 0x08, PIC_PKT_LEN & 0xff, (PIC_PKT_LEN >> 8) & 0xff, 0};
  unsigned char resp[6];

  Serial.setTimeout(100);
  while (1)
  {
    clearRxBuf();
    sendCmd(cmd, 6);
    if (Serial.readBytes((char *)resp, 6) != 6)
      continue;
    if (resp[0] == 0xaa && resp[1] == (0x0e | cameraAddr) && resp[2] == 0x06 && resp[4] == 0 && resp[5] == 0)
      break;
  }
  cmd[1] = 0x05 | cameraAddr;
  cmd[2] = 0;
  cmd[3] = 0;
  cmd[4] = 0;
  cmd[5] = 0;

  while (1)
  {
    clearRxBuf();
    sendCmd(cmd, 6);
    if (Serial.readBytes((char *)resp, 6) != 6)
      continue;
    if (resp[0] == 0xaa && resp[1] == (0x0e | cameraAddr) && resp[2] == 0x05 && resp[4] == 0 && resp[5] == 0)
      break;
  }
  cmd[1] = 0x04 | cameraAddr;
  cmd[2] = 0x1;

  while (1)
  {
    clearRxBuf();
    sendCmd(cmd, 6);
    if (Serial.readBytes((char *)resp, 6) != 6)
      continue;
    if (resp[0] == 0xaa && resp[1] == (0x0e | cameraAddr) && resp[2] == 0x04 && resp[4] == 0 && resp[5] == 0)
    {
      Serial.setTimeout(1000);
      if (Serial.readBytes((char *)resp, 6) != 6)
        continue;
      if (resp[0] == 0xaa && resp[1] == (0x0a | cameraAddr) && resp[2] == 0x01)
      {
        picTotalLen = (resp[3]) | (resp[4] << 8) | (resp[5] << 16);
        Serial.print(F("picTotalLen:"));
        Serial.println(picTotalLen);
        break;
      }
    }
  }
}

void handleNewMessages(int numNewMessages)
{
  for (int i = 0; i < numNewMessages; i++)
  {
    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;
    Serial.print(F("Bot text message: "));
    Serial.println(text);
    if (text.startsWith("/photo"))
      sendSnapshot(chat_id);
    if (text.startsWith("/start"))
    {
      bot.sendMessage(chat_id, "Hello, " + bot.messages[i].from_name + " " + bot.messages[i].from_id + " from " + chat_id, "");
    }
  }
}


void setup()
{
  Serial.begin(115200);
  Serial.println();

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

  initializeCam();
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
