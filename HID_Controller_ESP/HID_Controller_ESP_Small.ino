#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WebServer.h>
#include <EEPROM.h>
#include <UniversalTelegramBot.h>
#include <time.h>
#include "webpage.h"

WiFiServer server(80);

#define BOT_TOKEN "INSERT BOT TOKEN HERE" //Telegram Bot Credentials
#define CHAT_ID "INSERT CHAT ID HERE"
const unsigned long BOT_MTBS = 900; // mean time between scan messages

WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
unsigned long bot_lasttime; // last time messages' scan has been done

int Autostate;
int Photostate;
int ignState;
int i = 0;
int p = 0;
int DELAY = 900;
int limit;

int beep = 0;
int Status = 1;
int lampState;

int timezone = 7 * 3600;
int dst = 0;

bool OFF = LOW; //Main 4 relays
bool ON = HIGH;
bool nON = HIGH; //Additional 2 relays
bool nOFF = LOW;

String Wattage;

#define BlTemp 35      // Ballast Temperature
#define Photo  32     // Photoresistor
#define LaTemp 34     // Lamp Compartment Temperature

// Temperature ADC calibration.
#define TEMP_ADC_0C    2000
#define TEMP_ADC_100C  150

#define red 21        //RGB Led
#define green  22
#define blue  23
#define ignPin  15     // Ignitor
#define one  2       // 18 Watt Ballast
#define two  0       // 36 Watt Ballast #1 s
#define three 16     // 36 Watt Ballast #2
#define four  4       // 36 Watt Ballast #3
#define five  17      // 36 Watt Ballast #4

int A = 300;       // Rain
int B = 1000;       // 5 30 PM
int C = 2700;       // 6PM
int Dif = 100;     // Diff Value 

int w = 0;

void WifiHunt() {  //Wifi Cedentials, use it as much as you can. Make sure your Home Wifi is on the top so it quickly connects the luminaire.
  const char* ssidList[] = {
    "SSID1",
    "SSID2",
    "SSID3"
  };

  const char* passList[] = {
    "Password1",
    "Password2",
    "Password3"
  };

  const int wifiCount = sizeof(ssidList) / sizeof(ssidList[0]);

  Serial.print("Connecting to ");
  Serial.print(w);
  Serial.print(". ");
  Serial.println(ssidList[w]);

  WiFi.begin(ssidList[w], passList[w]);

  unsigned long wifiStartTime = millis();

  while (WiFi.status() != WL_CONNECTED) {

    if (millis() - wifiStartTime >= 10000) {

      wifiStartTime = millis();

      if (w + 1 < wifiCount) {
        w++;
      } else {
        w = 0;
      }

      Serial.println();
      Serial.print("10 seconds elapsed.");
      Serial.print(" Switching to WiFi ");
      Serial.print(w);
      Serial.print(". ");
      Serial.println(ssidList[w]);

      WiFi.disconnect(true);
      delay(100);

      WiFi.begin(ssidList[w], passList[w]);
    }

    if (millis() / 60000 > p) {
      p++;
    }

    if (p < 3) {
      Red();
    } else {
      if (Photostate == 0) {
        Magenta();
        Photoresistor();
      } else if (Photostate == 1) {
        Red();
      }
    }

    Serial.print("p = ");
    Serial.print(p);
    Serial.print(" | WiFi = ");
    Serial.print(w);
    Serial.print(" | Status = ");
    Serial.println(WiFi.status());

    delay(100);
  }

  Serial.println();
  Serial.print("WiFi connected to ");
  Serial.println(ssidList[w]);

  server.begin();
  Serial.println("Server started");

  if (Autostate == 0) {

    if (p >= 3) {

      if (Photostate == 0) {

        Green();
        Photoresistor();

      } else if (Photostate == 1) {

        if (Status == 1) Blue();
        else if (Status >= 2 && Status <= 8) Yellow();
        else if (Status == 9) White();
        else if (Status == 11) Green();
      }

    } else {

      if (Status == 1) Blue();
      else if (Status >= 2 && Status <= 8) Yellow();
      else if (Status == 9) White();
      else if (Status == 11) Green();
    }

  } else if (Autostate == 1) {

    Status = 11;
    Cyan();
    Auto();

  } else {

    if (p >= 3) {

      Status = 10;
      Green();
      Photoresistor();

    } else {

      if (Status == 1) Blue();
      else if (Status >= 2 && Status <= 8) Yellow();
      else if (Status == 9) White();
      else if (Status == 11) Green();
    }

    EEPROM.write(0, 0);
  }

  Serial.print("IP Address of network: ");
  Serial.println(WiFi.localIP().toString());

  Serial.print("Copy and paste the following URL: http://");
  Serial.print(WiFi.localIP().toString());
  Serial.println("/");

  Serial.println("\nConfiguring NTP...");

  configTime(
    7 * 3600,
    0,
    "pool.ntp.org",
    "time.nist.gov"
  );

  Serial.println("Waiting for Internet time");

  time_t now = time(nullptr);
  unsigned long startTime = millis();

  while (now < 24 * 3600 && millis() - startTime < 15000) {

    Serial.print("*");

    delay(500);

    now = time(nullptr);
  }

  if (now >= 24 * 3600) {

    Serial.println("\nTime response....OK");

  } else {

    Serial.println("\nNTP time synchronization FAILED");
  }

  #if defined(ESP32)

    secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT);

  #elif defined(ESP8266)

    static X509List cert(TELEGRAM_CERTIFICATE_ROOT);
    secured_client.setTrustAnchors(&cert);

  #endif
}

void setup() {
Serial.begin(115200);
#if defined(ESP32)
   analogReadResolution(12);
#endif
EEPROM.begin(512);
  Autostate = EEPROM.read(0);
  if (Autostate > 1) { Autostate = 1; EEPROM.write(0,0); EEPROM.commit(); }
  
  Photostate = EEPROM.read(1);
  if (Photostate > 1) { Photostate = 1; EEPROM.write(1,1); EEPROM.commit(); }
  
  ignState = EEPROM.read(3);
  if (ignState > 1) { ignState = 1; EEPROM.write(3,1); EEPROM.commit(); }
  
  limit = EEPROM.read(4);
  if (limit > 8 || limit < 4) { limit = 4; EEPROM.write(4,4); EEPROM.commit(); }
  
  pinMode(Photo, INPUT);
  pinMode(one, OUTPUT);
  pinMode(two, OUTPUT);
  pinMode(three, OUTPUT);
  pinMode(four, OUTPUT);
  pinMode(five, OUTPUT);
  pinMode(ignPin, OUTPUT);

  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
 
  digitalWrite(one, nOFF);
  digitalWrite(two, OFF);
  digitalWrite(three, OFF);
  digitalWrite(four, OFF);
  digitalWrite(five, OFF);
  digitalWrite(ignPin, nOFF);

  WifiHunt();
  Serial.println("Testing Telegram connection...");

  if (bot.getMe()) {
    Serial.println("Telegram BOT connection OK");
  } else {
    Serial.println("Telegram BOT connection FAILED");
  }
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    WifiHunt();
  } else {  
    static unsigned long lastAutoCheck = 0;     // Continuous background check for modes (every 1 second)
    if (millis() - lastAutoCheck > 1000) {
      lastAutoCheck = millis();
      if (Status == 10) {
        Photoresistor();
      } else if (Status == 11) {
        Auto();
      }
    }
    
    if (millis() - bot_lasttime > BOT_MTBS) {     // Telegram Bot check (non-blocking)
      int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

      Serial.print("Telegram updates: ");
      Serial.println(numNewMessages);
      while (numNewMessages) {
        Serial.println("got response");
        handleNewMessages(numNewMessages);
        numNewMessages = bot.getUpdates(bot.last_message_received + 1);
      }
      bot_lasttime = millis();
    }
    
    WiFiClient client = server.available();   //  Web Server check (non-blocking)
    if (client) {
      Serial.println("Waiting for new client");
      while (client.connected() && !client.available()) {
        delay(1);
      }

      String request = client.readStringUntil('\r');
      Serial.println(request);
      client.flush();

      // Web API endpoints
      if (request.indexOf("GET /mode?value=") != -1) {
        int pos = request.indexOf("GET /mode?value=") + 16;
        int value = request.substring(pos).toInt();

        if (value >= 1 && value <= 11) {
          Status = value;
          if (Status == 11) {
            Autostate = 1;
            EEPROM.write(0, 1);
          } else {
            Autostate = 0;
            EEPROM.write(0, 0);
          }
          EEPROM.commit();

          switch (Status) {
            case 1: SHUT();    Blue();   lampState = 0; break;
            case 2: FIRST();   Yellow(); lampState = 1; break;
            case 3: SECOND();  Yellow(); lampState = 2; break;
            case 4: THIRD();   Yellow(); lampState = 3; break;
            case 5: FOURTH();  Yellow(); lampState = 4; break;
            case 6: FIFTH();   Yellow(); lampState = 5; break;
            case 7: SIXTH();   Yellow(); lampState = 6; break;
            case 8: SEVENTH(); Yellow(); lampState = 7; break;
            case 9: NINTH();   White();  lampState = 8; break;
            case 10: Green(); LightSensor(); break;
            case 11: Cyan(); Auto(); break;
          }
        }
      }

      if (request.indexOf("GET /photo?state=") != -1) {
        int pos = request.indexOf("GET /photo?state=") + 17;
        int value = request.substring(pos).toInt();
        if (value == 0 || value == 1) {
          Photostate = value;
          EEPROM.write(1, Photostate);
          EEPROM.commit();
        }
      }

      if (request.indexOf("GET /ignitor?state=") != -1) {
        int pos = request.indexOf("GET /ignitor?state=") + 19;
        int value = request.substring(pos).toInt();
        if (value == 0 || value == 1) {
          ignState = value;
          EEPROM.write(3, ignState);
          EEPROM.commit();
          
          Ignitor();
          switch (Status) {
            case 1: SHUT();    Blue();   lampState = 0; break;
            case 2: FIRST();   Yellow(); lampState = 1; break;
            case 3: SECOND();  Yellow(); lampState = 2; break;
            case 4: THIRD();   Yellow(); lampState = 3; break;
            case 5: FOURTH();  Yellow(); lampState = 4; break;
            case 6: FIFTH();   Yellow(); lampState = 5; break;
            case 7: SIXTH();   Yellow(); lampState = 6; break;
            case 8: SEVENTH(); Yellow(); lampState = 7; break;
            case 9: NINTH();   White();  lampState = 8; break;
            case 10: Green(); LightSensor(); break;
            case 11: Cyan(); Auto(); break;
          }
        }
      }

      if (request.indexOf("GET /limit?value=") != -1) {
        int pos = request.indexOf("GET /limit?value=") + 17;
        int value = request.substring(pos).toInt();
        if (value >= 4 && value <= 8) {
          limit = value;
          EEPROM.write(4, limit);
          EEPROM.commit();
        }
      }

      // Telemetry endpoint
      if (request.indexOf("GET /api/status") != -1) {
        int ballastRaw = analogRead(BlTemp);
        int lampRaw = analogRead(LaTemp);

        float ballastTemp = 0.0f;
        float lampTemp = 0.0f;

        if (TEMP_ADC_100C != TEMP_ADC_0C) {
          ballastTemp = (float)(ballastRaw - TEMP_ADC_0C) * 100.0f / (float)(TEMP_ADC_100C - TEMP_ADC_0C);
          lampTemp = (float)(lampRaw - TEMP_ADC_0C) * 100.0f / (float)(TEMP_ADC_100C - TEMP_ADC_0C);
        }

        time_t now = time(nullptr);
        struct tm* p_tm = localtime(&now);
        char timeBuffer[16] = "00:00:00";

        if (p_tm) strftime(timeBuffer, sizeof(timeBuffer), "%H:%M:%S", p_tm);

        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: application/json");
        client.println("Cache-Control: no-cache");
        client.println("Access-Control-Allow-Origin: *");
        client.println();
        client.print("{\"status\":"); client.print(Status);
        client.print(",\"lampState\":"); client.print(lampState);
        client.print(",\"photostate\":"); client.print(Photostate);
        client.print(",\"ignState\":"); client.print(ignState);
        client.print(",\"limit\":"); client.print(limit);
        client.print(",\"ballastTemp\":"); client.print(ballastTemp, 1);
        client.print(",\"lampTemp\":"); client.print(lampTemp, 1);
        client.print(",\"ballastRaw\":"); client.print(ballastRaw);
        client.print(",\"lampRaw\":"); client.print(lampRaw);
        client.print(",\"time\":\""); client.print(timeBuffer);
        client.println("\"}");

        client.stop();
        return; 
      }

      //------------------HTML Page Creation---------------------//
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/html; charset=UTF-8");
      client.println("Cache-Control: no-cache");
      client.println("Connection: close");
      client.println();
      client.print(webpage);
      client.stop();
    }
  }
}

// --------------------------- MANUAL --------------------------- //

void SHUT() { // OFF
  Wattage = "OFF";
  digitalWrite(one, nOFF); digitalWrite(two, OFF); digitalWrite(three, OFF); digitalWrite(four, OFF); digitalWrite(five, OFF); digitalWrite(ignPin, nOFF);
  Serial.println(Wattage);
}
void FIRST() { // 18 Watt
  Wattage = "18 Watt";
  digitalWrite(one, nON); digitalWrite(two, OFF); digitalWrite(three, OFF); digitalWrite(four, OFF); digitalWrite(five, OFF);
  Serial.println(Wattage); Ignitor();
}
void SECOND() { // 36 Watt
  Wattage = "36 Watt";
  digitalWrite(one, nOFF); digitalWrite(two, ON); digitalWrite(three, OFF); digitalWrite(four, OFF); digitalWrite(five, OFF);
  Serial.println(Wattage); Ignitor();
}
void THIRD() { // 54 Watt
  Wattage = "54 Watt";
  digitalWrite(one, nON); digitalWrite(two, ON); digitalWrite(three, OFF); digitalWrite(four, OFF); digitalWrite(five, OFF);
  Serial.println(Wattage); Ignitor();
}
void FOURTH() { // 72 Watt
  Wattage = "72 Watt";
  digitalWrite(one, nOFF); digitalWrite(two, ON); digitalWrite(three, ON); digitalWrite(four, OFF); digitalWrite(five, OFF);
  Serial.println(Wattage); Ignitor();
}
void FIFTH() { // 90 Watt
  Wattage = "90 Watt";
  digitalWrite(one, nON); digitalWrite(two, ON); digitalWrite(three, ON); digitalWrite(four, OFF); digitalWrite(five, OFF);
  Serial.println(Wattage); Ignitor();
}
void SIXTH() { // 108 Watt
  Wattage = "108 Watt";
  digitalWrite(one, nOFF); digitalWrite(two, ON); digitalWrite(three, ON); digitalWrite(four, ON); digitalWrite(five, OFF);
  Serial.println(Wattage); Ignitor();
}
void SEVENTH() { // 126 Watt
  Wattage = "126 Watt";
  digitalWrite(one, nON); digitalWrite(two, ON); digitalWrite(three, ON); digitalWrite(four, ON); digitalWrite(five, OFF);
  Serial.println(Wattage); Ignitor();
}
void EIGHTH() { // 144 Watt
  Wattage = "144 Watt";
  digitalWrite(one, nOFF); digitalWrite(two, ON); digitalWrite(three, ON); digitalWrite(four, ON); digitalWrite(five, ON);
  Serial.println(Wattage); Ignitor();
}
void NINTH() { // 162 Watt
  Wattage = "162 Watt";
  digitalWrite(one, nON); digitalWrite(two, ON); digitalWrite(three, ON); digitalWrite(four, ON); digitalWrite(five, ON);
  Serial.println(Wattage); Ignitor();
}

// --------------------------- Indicator Color --------------------------- //

void Red() { digitalWrite(red, HIGH); digitalWrite(green, LOW); digitalWrite(blue, LOW); }
void Yellow() { digitalWrite(red, HIGH); digitalWrite(green, HIGH); digitalWrite(blue, LOW); }
void Green() { digitalWrite(red, LOW); digitalWrite(green, HIGH); digitalWrite(blue, LOW); }
void Cyan() { digitalWrite(red, LOW); digitalWrite(green, HIGH); digitalWrite(blue, HIGH); }
void Blue() { digitalWrite(red, LOW); digitalWrite(green, LOW); digitalWrite(blue, HIGH); }
void Magenta() { digitalWrite(red, HIGH); digitalWrite(green, LOW); digitalWrite(blue, HIGH); }
 void White() { digitalWrite(red, HIGH); digitalWrite(green, HIGH); digitalWrite(blue, HIGH); }

// --------------------------- Light Sensor --------------------------- //

void LightSensor() {
  Serial.println("Photoresistor Mode");
  beep = 2;

  static unsigned long previousMillis = 0;
  const unsigned long interval = 1000;

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    Photoresistor();
  }
}


// --------------------------- Automatic --------------------------- //

void Auto() {
  time_t now = time(nullptr);
  struct tm* p_tm = localtime(&now);

  if (p_tm->tm_hour == 0) {                           // 12 AM
      switch (limit) { 
        case 4: FOURTH(); break;
        case 5: FIFTH(); break;
        case 6: SIXTH(); break;
        case 7: SEVENTH(); break;
        case 8: NINTH(); break;
      }
  } else if ((p_tm->tm_hour >= 1) && (p_tm->tm_hour <= 4)) {  // 1 AM - 4 AM
      switch (limit) { 
        case 4: FOURTH(); break;
        case 5: case 6: case 7: case 8: FIFTH(); break;
      }
  } else if (p_tm->tm_hour == 5) {                     // 5 AM
    if (p_tm->tm_min <= 29) {                          
      FOURTH();
    } else if (p_tm->tm_min >= 30) {                    
      THIRD();
    }
  } else if (p_tm->tm_hour == 6) {                     // 6 AM
    if (p_tm->tm_min <= 29) {                          
      FOURTH();
    } else if (p_tm->tm_min >= 30) {                    
      SHUT();
    }
  } else if ((p_tm->tm_hour >= 7) && (p_tm->tm_hour <= 16)) {  // 7 AM - 4 PM
    SHUT();                                     
  } else if (p_tm->tm_hour == 17) {                    // 5 PM
    if (p_tm->tm_min <= 29) {                          
      SHUT();
    } else if (p_tm->tm_min >= 30) {                    
      THIRD();
    }
  } else if ((p_tm->tm_hour >= 18) && (p_tm->tm_hour <= 20)) { // 6 PM - 8 PM
    FOURTH();
  } else if ((p_tm->tm_hour >= 21) && (p_tm->tm_hour <= 23)) { // 9 PM - 11 PM
      switch (limit) { 
        case 4: FOURTH(); break;
        case 5: FIFTH(); break;
        case 6: case 7: case 8: SIXTH(); break;
      }
  }
}

void Photoresistor() {
  int Photores = analogRead(Photo);

  Serial.print("Photoresistor: ");
  Serial.println(Photores);

                          
  if (Photores <= A) {    // Sunny Day
    SHUT();
  }

  else if (Photores > A + Dif && Photores <= B) {    // Raining
    THIRD();
  }

  else if (Photores > B + Dif && Photores <= C) {    // 5.30 PM 
    switch (limit) {
      case 4:
        FOURTH();
        break;

      case 5:
      case 6:
      case 7:
      case 8:
        FIFTH();
        break;
    }
  } 
  else if (Photores > C + Dif) {   // 6 PM
    switch (limit) {
      case 4:
        FOURTH();
        break;

      case 5:
        FIFTH();
        break;

      case 6:
        SIXTH();
        break;

      case 7:
        SEVENTH();
        break;

      case 8:
        NINTH();
        break;
    }
  }
}


void Ignitor() {
 if (ignState == 1) {
    digitalWrite(ignPin, nON);
 } else if (ignState == 0) { 
    digitalWrite(ignPin, nOFF);
 }    
}

void handleNewMessages(int numNewMessages) {
  WiFiClient client = server.available();
  Serial.print("handleNewMessages ");

  for (int i = 0; i < numNewMessages; i++) {
    String text = bot.messages[i].text;

    if (text == "/AutoReset") {
      if (Photostate == 1) {  
        bot.sendMessage(CHAT_ID, "Street Light is automaticaly reset", "");
        Photostate = 0;
        EEPROM.write(1,0);
        EEPROM.commit();
      } else if (Photostate == 0) {  
        bot.sendMessage(CHAT_ID, "Street Light is always on static", "");
        Photostate = 1;
        EEPROM.write(1,1);
        EEPROM.commit();
      }     
    }

    if (text == "/Ignitor") {
      if (ignState == 1) {  
        bot.sendMessage(CHAT_ID, "Ignitor is disabled", "");
        ignState = 0;
        EEPROM.write(3,0);
        EEPROM.commit();
      } else if (ignState == 0) {  
        bot.sendMessage(CHAT_ID, "Ignitor is enabled", "");
        ignState = 1;
        EEPROM.write(3,1);
        EEPROM.commit();
      }
      switch (Status) { 
        case 2: FIRST(); break;
        case 3: SECOND(); break;
        case 4: THIRD(); break;
        case 5: FOURTH(); break;
        case 6: FIFTH(); break;
        case 7: SIXTH(); break;
        case 8: SEVENTH(); break;
        case 9: NINTH(); break;
      }  
    }

    if (text == "/OFF") {
      bot.sendMessage(CHAT_ID, "Street Light is turned off", "");
      SHUT(); EEPROM.write(0,0); EEPROM.commit(); Blue(); Status = 1;
    }

    if (text == "/18Watt") {
      bot.sendMessage(CHAT_ID, "Current power of Street Light is 18 Watt", "");
      FIRST(); EEPROM.write(0,0); EEPROM.commit(); Yellow(); Status = 2;
    }
    if (text == "/36Watt") {
      bot.sendMessage(CHAT_ID, "Current power of Street Light is 36 Watt", "");
      SECOND(); EEPROM.write(0,0); EEPROM.commit(); Yellow(); Status = 3;
    }
    if (text == "/54Watt") {
      bot.sendMessage(CHAT_ID, "Current power of Street Light is 54 Watt", "");
      THIRD(); EEPROM.write(0,0); EEPROM.commit(); Yellow(); Status = 4;
    }
    if (text == "/72Watt") {
      bot.sendMessage(CHAT_ID, "Current power of Street Light is 72 Watt", "");
      FOURTH(); EEPROM.write(0,0); EEPROM.commit(); Yellow(); Status = 5;
    }
    if (text == "/90Watt") {
      bot.sendMessage(CHAT_ID, "Current power of Street Light is 90 Watt", "");
      FIFTH(); EEPROM.write(0,0); EEPROM.commit(); Yellow(); Status = 6;
    }
    if (text == "/108Watt") {
      bot.sendMessage(CHAT_ID, "Current power of Street Light is 108 Watt", "");
      SIXTH(); EEPROM.write(0,0); EEPROM.commit(); Yellow(); Status = 7;
    }
    if (text == "/126Watt") {
      bot.sendMessage(CHAT_ID, "Current power of Street Light is 126 Watt", "");
      SEVENTH(); EEPROM.write(0,0); EEPROM.commit(); Yellow(); Status = 8;
    }
    if (text == "/162Watt") {
      bot.sendMessage(CHAT_ID, "Current power of Street Light is 162 Watt", "");
      NINTH(); EEPROM.write(0,0); EEPROM.commit(); White(); Status = 9;
    }
    if (text == "/Photoresistor") {
      bot.sendMessage(CHAT_ID, "Current state of Street Light is in Photoresistor mode", "");
      LightSensor(); EEPROM.write(0,0); EEPROM.commit(); Green(); Status = 10;
    }
    if (text == "/Auto") {
      bot.sendMessage(CHAT_ID, "Current state of Street Light is in Automatic mode", "");
      Auto(); EEPROM.write(0,1); EEPROM.commit(); Cyan(); Status = 11;
    }
    if (text.equalsIgnoreCase("/status")) {
      switch (Status) {
        case 1: bot.sendMessage(CHAT_ID, "Street Light is turned off", ""); break;
        case 2: bot.sendMessage(CHAT_ID, "Current power of Street Light is 18 Watt", ""); break;
        case 3: bot.sendMessage(CHAT_ID, "Current power of Street Light is 36 Watt", ""); break;
        case 4: bot.sendMessage(CHAT_ID, "Current power of Street Light is 54 Watt", ""); break;
        case 5: bot.sendMessage(CHAT_ID, "Current power of Street Light is 72 Watt", ""); break;
        case 6: bot.sendMessage(CHAT_ID, "Current power of Street Light is 90 Watt", ""); break;
        case 7: bot.sendMessage(CHAT_ID, "Current power of Street Light is 108 Watt", ""); break;
        case 8: bot.sendMessage(CHAT_ID, "Current power of Street Light is 126 Watt", ""); break;
        case 9: bot.sendMessage(CHAT_ID, "Current power of Street Light is 162 Watt", ""); break;
        case 10: bot.sendMessage(CHAT_ID, "Current state of Street Light is in Photoresistor mode: " + Wattage, ""); break;
        case 11: bot.sendMessage(CHAT_ID, "Current state of Street Light is in Automatic mode: " + Wattage, ""); break;
        default: bot.sendMessage(CHAT_ID, "Street Light is turned off", "");
      }
    }
    if (text.equalsIgnoreCase("/start")) {
      String welcome = "Welcome to 1980 Philips HID Street Light HRC 502.\n";
      Serial.println(WiFi.localIP().toString());
      welcome += "You can use browser to control the street light by using this IP: " + WiFi.localIP().toString() + ".\n";
      welcome += "Or use this command to control directly:\n";
      welcome += "/OFF : to turn off Lamp.\n";
      welcome += "/18Watt: to run at 18 Watt.\n";
      welcome += "/36Watt: to run at 36 Watt.\n";
      welcome += "/54Watt: to run at 54 Watt.\n";
      welcome += "/72Watt: to run at 72 Watt.\n";
      welcome += "/90Watt: to run at 90 Watt.\n";
      welcome += "/108Watt: to run at 108 Watt.\n";
      welcome += "/126Watt: to run at 126 Watt.\n";
      welcome += "/162Watt: to run at 162 Watt.\n";
      welcome += "/Photoresistor: to set Photoresistor mode.\n";
      welcome += "/Auto: to set Automatic mode.\n";
      welcome += "/AutoReset : to turn streetlight static or photoresistor after 3 min. \n";
      welcome += "/Ignitor : to either enable or disable the ignitor. \n";
      bot.sendMessage(CHAT_ID, welcome, "Markdown");
    }
  }
}

// 2019 - 2026 By Bahyyazid Ramadhan Hendarto - The Beloved Project
