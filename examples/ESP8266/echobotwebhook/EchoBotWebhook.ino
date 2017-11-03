/*******************************************************************
*  An example of bot that echos back any messages received         *
*  Webhook edition                                                 *
*                                                                  *
*  Webhook example by Denis G Dugushkin                            *                                                                  
*  written by Giacarlo Bacchio (Gianbacchio on Github)             *
*  adapted by Brian Lough                                          *
*******************************************************************/
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

// Initialize Wifi connection to the router
char ssid[] = "****************";     // your network SSID (name)
char password[] = "****************"; // your network key

// Initialize Telegram BOT
#define BOTtoken "****************"  // your Bot Token (Get from Botfather)
#define secretURI BOTtoken

// Here is generated SSL certificate and private key

static const uint8_t x509[]  ICACHE_RODATA_ATTR  = { // The certificate is stored in PMEM
  #include "x509.h" // YOU SHOULD GENERATE YOUR OWN CERTIFICATE. USE SPECIAL SCRIPT TO DO IT
};

static const uint8_t rsakey[]  ICACHE_RODATA_ATTR = { // And so is the key.  These could also be in PMEM
  #include "key.h" // YOU SHOULD GENERATE YOUR OWN PRIVATE KEY. USE SPECIAL SCRIPT TO DO IT
};

WiFiServerSecure server(8443);


UniversalTelegramBot bot(BOTtoken, secretURI);


void setup() {
  Serial.begin(115200);

  // Set WiFi to station mode and disconnect from an AP if it was Previously
  // connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // Attempt to connect to Wifi network:
  Serial.println("");
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // SSL Server initialization
  server.setServerKeyAndCert_P(rsakey, sizeof(rsakey), x509, sizeof(x509));
  server.begin();
  Serial.println("HTTPS server started");
}

void loop() {
    WiFiClientSecure client;
    
    client = server.available();
    
    if (client) {
    Serial.println("Telegram client connected");
    
    while(client.connected() ) {
    delay(10);
      
      int numNewMessages = bot.getUpdates(client);
      if (numNewMessages > 0) {
      Serial.println("got response: "+ String(numNewMessages));  
      
      for (int i=0; i<numNewMessages; i++) {
        Serial.println("answer #"+String(i));  
        Serial.println(bot.messages[i].text);
        bot.sendMessage(bot.messages[i].chat_id, bot.messages[i].text, "");
      }
     } else Serial.println("Nothing to do");
       
     }
    client.stop();
    Serial.println("Client disconnected");
    }
}
