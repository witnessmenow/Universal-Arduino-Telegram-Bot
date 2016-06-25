/*******************************************************************
 *  An example of bot that echos back any messages received        *
 *  using 101TelegramBOT.                                          *
 *                                                                 *
 *  written by Brian Lough                                         *
 *******************************************************************/


 #include <WiFi101.h>
 #include <101TelegramBOT.h>


// Initialize Wifi connection to the router
char ssid[] = "xxxxxxxxxxxxxxxxxxxxxxxx";              // your network SSID (name)
char password[] = "yyyyyyyyy";                              // your network key


// Initialize Telegram BOT
#define BOTtoken "XXXXXXXXX:XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"  // your Bot Token (Get off Botfather)
101TelegramBOT bot(BOTtoken);

int Bot_mtbs = 1000; //mean time between scan messages
long Bot_lasttime;   //last time messages' scan has been done


/********************************************
 * EchoMessages - function to Echo messages *
 ********************************************/
void Bot_EchoMessages(int numNewMessages) {
  for(int i=0; i<numNewMessages; i++) {
    StaticJsonBuffer<500> jsonBuffer;
    JsonObject& payload = jsonBuffer.createObject();
    payload["chat_id"] = bot.messages[i].chat_id;
    payload["text"] = bot.messages[i].text;
    bot.sendPostMessage(payload);
  }
}


void setup() {

  Serial.begin(115200);
  delay(3000);

  // attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);

  bot.begin();      // launch Bot functionalities
}



void loop() {

  if (millis() > Bot_lasttime + Bot_mtbs)  {
    int numNewMessages = bot.getUpdates(bot.last_message_recived + 1);   // launch API GetUpdates up to xxx message
    if(numNewMessages) {
      Serial.println("got response");
      Bot_EchoMessages(numNewMessages);   // reply to message with Echo
    }
    Bot_lasttime = millis();
  }
}
