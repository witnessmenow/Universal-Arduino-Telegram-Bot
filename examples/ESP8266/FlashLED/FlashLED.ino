/*******************************************************************
 *  An example of bot that receives commands and turns on and off  *
 *  an LED.                                                        *
 *                                                                 *
 *  written by Giacarlo Bacchio (Gianbacchio on Github)            *
 *  adapted by Brian Lough                                         *
 *******************************************************************/
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <TimeLib.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include "FS.h"
#include <AccelStepper.h>
#include "DHT.h"
#include <ArduinoOTA.h>
#include <UniversalTelegramBot.h>

// Initialize Wifi connection to the router
char ssid[] = "1a3496";        // your network SSID (name)
char password[] = "278833924"; // your network key

// Initialize Telegram BOT
#define BOTtoken "326858637:AAGDZGkaKP_Vrhzs8mkd--0BnGQQ6AioF4M"  // your Bot Token (Get off Botfather)

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

int Bot_mtbs = 1000; //mean time between scan messages
long Bot_lasttime;   //last time messages' scan has been done
bool Start = false;

const int ledPin = 13;
int ledStatus = 0;




// PINS SETUP
#define REED_WINDOW_CLOSED D1
#define REED_WINDOW_MAX_OPEN D2
#define STEPPER_DRIVER_ON_OFF D3
#define DHTPIN D7
#define RESET_CONFIG_BUTTON D8

// TEMP SETUP
#define DHTTYPE DHT22

String apiKey = "xxxxxxxxxxxxx";
const char* statistic_server = "api.thingspeak.com";

int wifi_connect_tries = 0;

const int is_in_office = 0;
const int is_in_dev_room = 0;

// sleep for this many seconds
const int sleepSeconds = 10;

AccelStepper Stepper1(1,D5,D6);
WiFiServer server(80);

String setup_log = "";
String loop_log = "";

// NTP Servers:
static const char ntpServerName[] = "us.pool.ntp.org";
//static const char ntpServerName[] = "time.nist.gov";
//static const char ntpServerName[] = "time-a.timefreq.bldrdoc.gov";
//static const char ntpServerName[] = "time-b.timefreq.bldrdoc.gov";
//static const char ntpServerName[] = "time-c.timefreq.bldrdoc.gov";

const int timeZone = 2;     // Central European Time

WiFiUDP Udp;
unsigned int localPort = 8888;  // local port to listen for UDP packets

time_t getNtpTime();
void digitalClockDisplay();
void printDigits(int digits);
void sendNTPpacket(IPAddress &address);

DHT dht(DHTPIN, DHTTYPE);

int automatic_mode = 1;
float current_window_degree = 0;
int automatic_window_step = 2; //degrees
int dir_open = -1;
int dir_close = 1;

int keep_temp_cold_room = 23; // (+-1)
time_t last_motor_action;

bool motor_working = false;
String current_motor_action = "";
bool debug = false;
bool config_found = true;
bool closing_window_config_not_found = false;
bool need_redirect_to_main_page = false;

String s = "LOG";

bool sketch_updating = false;




void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;

    String from_name = bot.messages[i].from_name;
    if (from_name == "") from_name = "Guest";

    if (text == "/ledon") {
      digitalWrite(ledPin, HIGH);   // turn the LED on (HIGH is the voltage level)
      ledStatus = 1;
      bot.sendMessage(chat_id, "Led is ON", "");
    }

    if (text == "/ledoff") {
      ledStatus = 0;
      digitalWrite(ledPin, LOW);    // turn the LED off (LOW is the voltage level)
      bot.sendMessage(chat_id, "Led is OFF", "");
    }

    if (text == "/status") {
      String file_content = "Ninja\n\n";

      file_content += "mode - Change mode (automatic or manual)\n";

      file_content += "open_window_1 - Open window by 1 degrees\n";
      file_content += "open_window_3 - Open window by 3 degrees\n";
      file_content += "open_window_5 - Open window by 5 degrees\n";
      file_content += "open_window_10 - Open window by 10 degrees\n\n";

      file_content += "close_window_1 - Close window by 1 degrees\n";
      file_content += "close_window_3 - Close window by 3 degrees\n";
      file_content += "close_window_5 - Close window by 5 degrees\n";
      file_content += "close_window_10 - Close window by 10 degrees\n\n";

      file_content += "mode - Change mode (automatic or manual)\n\n";

      file_content += "open_window_1 - Open window by 1 degrees\n";
      file_content += "open_window_3 - Open window by 3 degrees\n";
      file_content += "open_window_5 - Open window by 5 degrees\n";
      file_content += "open_window_10 - Open window by 10 degrees\n\n";

      file_content += "close_window_1 - Close window by 1 degrees\n";
      file_content += "close_window_3 - Close window by 3 degrees\n";
      file_content += "close_window_5 - Close window by 5 degrees\n";
      file_content += "close_window_10 - Close window by 10 degrees";

      bot.sendMessage(chat_id, file_content);
    }

    if (text == "/start") {
      String welcome = "Welcome to Universal Arduino Telegram Bot library, " + from_name + ".\n";
      welcome += "This is Flash Led Bot example.\n\n";
      welcome += "/ledon : to switch the Led ON\n";
      welcome += "/ledoff : to switch the Led OFF\n";
      welcome += "/status : Returns current status of LED\n";
      bot.sendMessage(chat_id, welcome, "Markdown");
    }
  }
}


void setup() {
  Serial.begin(115200);

  // Set WiFi to station mode and disconnect from an AP if it was Previously
  // connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // attempt to connect to Wifi network:
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

  pinMode(ledPin, OUTPUT); // initialize digital ledPin as an output.
  delay(10);
  digitalWrite(ledPin, LOW); // initialize pin as off
}

void loop() {
  if (millis() > Bot_lasttime + Bot_mtbs)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    Bot_lasttime = millis();
  }
}
