/*******************************************************************
 *  An example of bot that receives commands and turns on and off  *
 *  an led using using 101TelegramBOT.                             *
 *                                                                 *
 *  written by Giacarlo Bacchio (Gianbacchio on Github)            *
 *  adapted by Brian Lough                                         *
 *******************************************************************/

 #include <WiFi101.h>
 #include <101TelegramBOT.h>


// Initialize Wifi connection to the router
char ssid[] = "xxxxxxxxxxxxxxxxxxxxxx";              // your network SSID (name)
char pass[] = "yyyyyyyy";                              // your network key



const int ledPin = 13;

// Initialize Telegram BOT
#define BOTtoken "XXXXXXXXX:XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"  // your Bot Token (Get off Botfather)

101TelegramBOT bot(BOTtoken);

int Bot_mtbs = 1000; //mean time between scan messages
long Bot_lasttime;   //last time messages' scan has been done
bool Start = false;
int ledStatus = 0;

void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));
  for(int i=0; i<numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;
    if (text == "\/ledon") {
      digitalWrite(ledPin, HIGH);   // turn the LED on (HIGH is the voltage level)
      ledStatus = 1;
      bot.sendMessage(chat_id, "Led is ON", "");
    }
    if (text == "\/ledoff") {
      ledStatus = 0;
      digitalWrite(ledPin, LOW);    // turn the LED off (LOW is the voltage level)
      bot.sendMessage(chat_id, "Led is OFF", "");
    }
    if (text == "\/status") {
      if(ledStatus){
        bot.sendMessage(chat_id, "Led is ON", "");
      } else {
        bot.sendMessage(chat_id, "Led is OFF", "");
      }
    }
    if (text == "\/start") {
      String wellcome = "Wellcome from FlashLedBot, your personal Bot on ESP8266 board";
      String wellcome1 = "/ledon : to switch the Led ON";
      String wellcome2 = "/ledoff : to switch the Led OFF";
      String wellcome3 = "/status : Returns current status of LED";
      bot.sendMessage(chat_id, wellcome, "");
      bot.sendMessage(chat_id, wellcome1, "");
      bot.sendMessage(chat_id, wellcome2, "");
      bot.sendMessage(chat_id, wellcome3, "");
      Start = true;
    }
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
  pinMode(ledPin, OUTPUT); // initialize digital ledPin as an output.
  delay(10);
  digitalWrite(ledPin, HIGH); //initilase pin as off

}



void loop() {
  if (millis() > Bot_lasttime + Bot_mtbs)  {

    int numNewMessages = bot.getUpdates(bot.last_message_recived + 1);   // launch API GetUpdates up to xxx message
    if(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);   // reply to message with Echo
    }
    Bot_lasttime = millis();
  }
}
