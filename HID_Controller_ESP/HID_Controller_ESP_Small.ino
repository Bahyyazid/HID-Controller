#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <time.h>
#include <UniversalTelegramBot.h>
#include <EEPROM.h>


ESP8266WiFiMulti wifiMulti;



WiFiServer server(80);

#define BOT_TOKEN "INSERT YOUT BOT TOKEN HERE"
#define CHAT_ID "INSERT YOUT BOT TOKEN HERE"
const unsigned long BOT_MTBS = 900; // mean time between scan messages

X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
unsigned long bot_lasttime; // last time messages' scan has been done




int Autostate;
int Photostate;
int ignState;
int i = 0;
int p = 0;
int DELAY = 900;

int beep = 0;
int Status = 1;


int timezone = 7 * 3600;
int dst = 0;

bool OFF = HIGH;
bool ON = LOW;
bool nON = LOW;
bool nOFF = HIGH;

String Wattage;

#define red  D8
#define green  D6
#define blue  D7

#define one  D0       // 18 Watt Ballast
#define two  D1       // 36 Watt Ballast #1
#define three  D2     // 36 Watt Ballast #2
#define four  D3      // 36 Watt Ballast #3
#define five  D4      // 36 Watt Ballast #4
#define ignPin  D5    // Ignitor
#define Photo  A0     // Photoresistor

int A = 200;        //  6 PM
int B = 550;       // 5 30 PM
int C = 650;   //  Rain
int Dif = 100;    // Value 

int w = 0;



void WifiHunt(){
   //add more WiFi 
  const char* ssidList[] = {"WIFI NAME 1", "WIFI NAME 2", "ETC"}; //add WiFi Name Here
  const char* passList[] = {"WIFI NAME 1 PASSWORD", "WIFI NAME 2 PASSWORD", "ETC PASSWORD"}; //add WiFi Pass Here
  const int wifiCount = sizeof(ssidList) / sizeof(ssidList[0]);
  
  Serial.print("Connecting to ");
  Serial.print(w);
  Serial.print(". ");
  Serial.println(ssidList[w]);
  WiFi.begin(ssidList[w], passList[w]);
  for (i >=0 ; WiFi.status() != WL_CONNECTED ; i++) {
    if (i % 10 == 0){     //Find another WiFi if can't connect for 10 seconds
      if(w + 1 < wifiCount){
       w++;
      } else if (w + 1 == wifiCount){
       w = 0; 
      }
      WiFi.begin(ssidList[w], passList[w]); 
      if (p==3){
        Serial.print("Connecting to Wifi Failed, attempting connection to ");
        Serial.print(w);
        Serial.print(". ");
        Serial.println(ssidList[w]);
      }else if (p>3){
        Serial.print("Connecting to Wifi Failed, attempting connection to ");
        Serial.print(w);
        Serial.print(". ");
        Serial.println(ssidList[w]);
        Serial.println(" and switching to Photoresistor mode.");   
      }
      
    }
    if(i % 60 == 0){
       p++;
    }
    if (p<3){   //Offline
      Red(); 
    }else if(p==3){   //Switching to Photoresistor mode if being offline for 3 minutes.
      if(Photostate == 0){
        Magenta();
        Photoresistor();
      }else if (Photostate == 1){
        Red();  
      }
    }else if(p>3){
      if(Photostate == 0){
        Magenta();
        Photoresistor();
      }else if (Photostate == 1){
        Red();  
      } 
    }
    Serial.print(p);
    delay(900); 
  }
  Serial.print("WiFi connected to ");
  Serial.println(ssidList[w]);
  server.begin();  // Starts the Server
  Serial.println("Server started");

  if(Autostate == 0){
    if(p>=3){
      if(Photostate == 0){
        Green();
        Photoresistor();
      }else if (Photostate == 1){
        if (Status == 1){
          Blue(); //blue when online
        } else if (Status == 2 || Status == 3 || Status == 4 || Status == 5 || Status == 6 || Status == 7 || Status == 8){
          Yellow(); //yellow when online
        } else if (Status == 9){
          White(); //white when online
        }else if (Status == 11){
          Green(); //blue when online
        }  
      } 
    }else if (p<3){ 
      if (Status == 1){
      Blue(); //blue when online
      } else if (Status == 2 || Status == 3 || Status == 4 || Status == 5 || Status == 6 || Status == 7 || Status == 8){
      Yellow(); //yellow when online
      } else if (Status == 9){
      White(); //white when online
      }else if (Status == 11){
      Green(); //blue when online
      }
    }  
  }else if(Autostate == 1){
    Status = 11;
    Cyan(); //Cyan when online on Auto
    Auto();
  }else{
    if(p>=3){
      Status = 10;
      Green(); //green when online on phoresistor
      Photoresistor();
    }else if (p<3){ 
      if (Status == 1){
      Blue(); //blue when online
      } else if (Status == 2 || Status == 3 || Status == 4 || Status == 5 || Status == 6 || Status == 7 || Status == 8){
      Yellow(); //yellow when online
      } else if (Status == 9){
      White(); //white when online
      }else if (Status == 11){
      Green(); //blue when online;
      }
    }  
    EEPROM.write(0,0);
  }
  Serial.print("IP Address of network: "); // Prints IP address on Serial Monitor
  Serial.println(WiFi.localIP().toString());
  Serial.print("Copy and paste the following URL: https://");
  Serial.print(WiFi.localIP().toString());
  Serial.println("/");

  Serial.println("\nWaiting for Internet time");

  while (!time(nullptr)) {
    Serial.print("*");
    delay(900);
  }
  Serial.println("\nTime response....OK");
  configTime(7 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  secured_client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org
}

void setup()
{
  
  
  Serial.begin(115200);
  EEPROM.begin(512);
  Autostate = EEPROM.read(0);
  if (Autostate > 1){
    Autostate = 1;
    EEPROM.write(0,0);
    EEPROM.commit(); 
  }
  Photostate = EEPROM.read(1);
  if (Photostate > 1){
    Photostate = 1;
    EEPROM.write(1,1);
    EEPROM.commit(); 
  }
  ignState = EEPROM.read(3);
  if (ignState > 1){
    ignState = 1;
    EEPROM.write(3,1);
    EEPROM.commit(); 
  }
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
 
}


void loop() {
if (WiFi.status() != WL_CONNECTED){
  WifiHunt();
}else{  
  if (millis() - bot_lasttime > BOT_MTBS)
  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    if (Status == 10) {
      Photoresistor();
    } else if (Status == 11) {
      Auto();
    }
    while (numNewMessages)
    {
      Serial.println("got response");
      handleNewMessages(numNewMessages);

      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    bot_lasttime = millis();
  }

  WiFiClient client = server.available();
  if (!client)
  {
    return;
  }
  Serial.println("Waiting for new client");
  while (!client.available())
  {
    delay(1);
  }

  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();


  if (request.indexOf("/OFF") != -1) {
    SHUT();
    EEPROM.write(0,0);
    EEPROM.commit(); 
    Blue();
    Status = 1;
  }
  if (request.indexOf("/18Watt") != -1) {
    FIRST();
    EEPROM.write(0,0);
    EEPROM.commit(); 
    Yellow();
    Status = 2;
  }
  if (request.indexOf("/36Watt") != -1) {  
    SECOND();
    EEPROM.write(0,0);
    EEPROM.commit(); 
    Yellow();
    Status = 3;
  }
  if (request.indexOf("/54Watt") != -1) { 
    THIRD();
    EEPROM.write(0,0);
    EEPROM.commit(); 
    Yellow();
    Status = 4;
  }
  if (request.indexOf("/72Watt") != -1) { 
    FOURTH();
    EEPROM.write(0,0);
    EEPROM.commit(); 
    Yellow();
    Status = 5;
  }
  if (request.indexOf("/90Watt") != -1) { 
    FIFTH();
    EEPROM.write(0,0);
    EEPROM.commit(); 
    Yellow();
    Status = 6;
  }
  if (request.indexOf("/108Watt") != -1) {  
    SIXTH();
    EEPROM.write(0,0);
    EEPROM.commit(); 
    Yellow();
    Status = 7;
  }
  if (request.indexOf("/126Watt") != -1) {   
    SEVENTH();
    EEPROM.write(0,0);
    EEPROM.commit(); 
    Yellow();
    Status = 8;
  }
  if (request.indexOf("/162Watt") != -1) {
    NINTH();
    EEPROM.write(0,0);
    EEPROM.commit(); 
    White();
    Status = 9;
  }
  if (request.indexOf("/Photoresistor") != -1) {
    Green();
    EEPROM.write(0,0);
    EEPROM.commit();  
    Status = 10;

  }
  if (request.indexOf("/Auto") != -1) {
    Cyan();
    EEPROM.write(0,1);
    EEPROM.commit(); 
    Status = 11;
  }

  /*------------------HTML Page Creation---------------------*/

  client.println("HTTP/1.1 200 OK"); // standalone web server with an ESP8266
  client.println("Content-Type: text/html");
  client.println("");
  client.println("<!DOCTYPE html>");
  client.println("<html>");

  client.println("<head>");
  client.println("<title>Philips S/HRC 502 Street Light</title>");
  client.println("</head>");
  client.println("<body style=\"background-color: #111111\" position=\"center\">");
  client.println("<center class=\"Controller\">");
  client.println("<img class=\"logo\" src=\"https://i.ibb.co/0qq449c/title.png\" alt=\"\" height=\"200px\" Style =\"margin-left: 36px;\">");
  if (Status == 1) {
    Blue();
    client.println("<ul><a href=\"/OFF\"\"><form class=\"OFF\"><img src=\"https://i.ibb.co/JcV8df8/OFFPressed.png\" alt=\"\" height=\"80px\" border=\"none\"></button></a></ul>");
  } else {
    client.println("<ul><a href=\"/OFF\"\"><form class=\"OFF\"><img src=\"https://i.ibb.co/6H29kTh/OFF.png\" alt=\"\" height=\"80px\" border=\"none\"></button></a></ul>");
  } if (Status == 2) {
    Yellow();
    client.println("<ul><a href=\"/18Watt\"\"><form class=\"18Watt\"><img src=\"https://i.ibb.co/HqH6KhM/18-WPressed.png\" alt=\"\" height=\"80px\"></button></a></ul>");
  } else {
    client.println("<ul><a href=\"/18Watt\"\"><form class=\"18Watt\"><img src=\"https://i.ibb.co/mRZCx3x/18W.png\" alt=\"\" height=\"80px\"></button></a></ul>");
  } if (Status == 3) {
    Yellow();
    client.println("<ul><a href=\"/36Watt\"\"><form class=\"36Watt\"><img src=\"https://i.ibb.co/rHk9mVJ/36-WPressed.png\" alt=\"\" height=\"80px\"></button></a></ul>");
  } else {
    client.println("<ul><a href=\"/36Watt\"\"><form class=\"36Watt\"><img src=\"https://i.ibb.co/cT3SL6d/36W.png\" alt=\"\" height=\"80px\"></button></a></ul>");
  } if (Status == 4) {
    Yellow();
    client.println("<ul><a href=\"/54Watt\"\"><form class=\"54Watt\"><img src=\"https://i.ibb.co/kcdy2ks/54-WPressed.png\" alt=\"\" height=\"80px\"></button></a></ul>");
  } else {
    Yellow();
    client.println("<ul><a href=\"/54Watt\"\"><form class=\"54Watt\"><img src=\"https://i.ibb.co/QPtzXPz/54W.png\" alt=\"\" height=\"80px\"></button></a></ul>");
  } if (Status == 5) {
    Yellow();
    client.println("<ul><a href=\"/72Watt\"\"><form class=\"72Watt\"><img src=\"https://i.ibb.co/rmmMnKK/72-WPressed.png\"alt=\"\" height=\"80px\"></button></a></ul>");
  } else {
    Yellow();
    client.println("<ul><a href=\"/72Watt\"\"><form class=\"72Watt\"><img src=\"https://i.ibb.co/Y0Mp2Gk/72W.png\"alt=\"\" height=\"80px\"></button></a></ul>");
  } if (Status == 6) {
    Yellow();
    client.println("<ul><a href=\"/90Watt\"\"><form class=\"90Watt\"><img src=\"https://i.ibb.co/dcN0zRC/90-WPressed.png\" alt=\"\" height=\"80px\"></button></a></ul>");
  } else {
    client.println("<ul><a href=\"/90Watt\"\"><form class=\"90Watt\"><img src=\"https://i.ibb.co/DMDHjk3/90W.png\" alt=\"\" height=\"80px\"></button></a></ul>");
  } if (Status == 7) {
    Yellow();
    client.println("<ul><a href=\"/108Watt\"\"><form class=\"108Watt\"><img src=\"https://i.ibb.co/Q6ZgPbs/108-WPressed.png\" alt=\"\" height=\"80px\"></button></a></ul>");
  } else {
    client.println("<ul><a href=\"/108Watt\"\"><form class=\"108Watt\"><img src=\"https://i.ibb.co/Xp8bcyH/108W.png\" alt=\"\" height=\"80px\"></button></a></ul>");
  } if (Status == 8) {
    Yellow();
    client.println("<ul><a href=\"/126Watt\"\"><form class=\"126Watt\"><img src=\"https://i.ibb.co/H7s3yt0/126-WPressed.png\" alt=\"\" height=\"80px\"></button></a></ul>");
  } else {
    client.println("<ul><a href=\"/126Watt\"\"><form class=\"126Watt\"><img src=\"https://i.ibb.co/fMX2QHd/126W.png\" alt=\"\" height=\"80px\"></button></a></ul>");
  } if (Status == 9) {
    White();
    client.println("<ul><a href=\"/162Watt\"\"><form class=\"162Watt\"><img src=\"https://i.ibb.co/PFw8FwH/162-WPressed.png\" alt=\"\" height=\"80px\"></button></a></ul>");
  } else {
    client.println("<ul><a href=\"/162Watt\"\"><form class=\"162Watt\"><img src=\"https://i.ibb.co/Z1TpgV9/162W.png\" alt=\"\" height=\"80px\"></button></a></ul>");
  } if (Status == 10) {
    Green();
    client.println("<ul><a href=\"/Photoresistor\"\"><form class=\"Photoresistor\"><img src=\"https://i.ibb.co/k0g0cQP/Photocell-Activated.png\" alt=\"\" height=\"80px\"></button></a></ul>");
    Photoresistor();
    client.println("<script>");
    client.println("function autoRefresh() {");
    client.println("window.location = window.location.href;");
    client.println("}");
    client.println("setInterval('autoRefresh()', 900);");
    client.println("</script>");
  } else {
    client.println("<ul><a href=\"/Photoresistor\"\"><form class=\"Photoresistor\"><img src=\"https://i.ibb.co/VmxgZJQ/Photocell.png\" alt=\"\" height=\"80px\"></button></a></ul>");
  } if (Status == 11) {
    Cyan();
    client.println("<ul><a href=\"/Auto\"\"><form class=\"Auto\"><img src=\"https://i.ibb.co/2FCZLfw/Auto-Pressed.png\" alt=\"\" height=\"80px\"></form></a></ul>");
    Auto();
    client.println("<script>");
    client.println("function autoRefresh() {");
    client.println("window.location = window.location.href;");
    client.println("}");
    client.println("setInterval('autoRefresh()', 900);");
    client.println("</script>");
  } else {
    client.println("<ul><a href=\"/Auto\"\"><form class=\"Auto\"><img src=\"https://i.ibb.co/TcZHv0h/Auto.png\" alt=\"\" height=\"80px\"></form></a></ul>");
  } client.println("<img class=\"logo\" src=\"https://i.ibb.co/GHFqKts/name.png\" alt=\"\" height=\"150px\" Style =\"margin-left: 36px;\">");
  client.println("</center>");
  client.println("</body>");
  client.println("</html>");


  delay(1);
  Serial.println("Client disonnected");
  }
}
///////////////////////////////////////////////////////////////////MANUAL //////////////////////////////////////////////////////////////////////

void SHUT()   {                            // OFF
  Wattage = "OFF";
  digitalWrite(one, nOFF);
  digitalWrite(two, OFF);
  digitalWrite(three, OFF);
  digitalWrite(four, OFF);
  digitalWrite(five, OFF);
  digitalWrite(ignPin, nOFF);
  Serial.println(Wattage);

}

void FIRST()  {                            // 18 Watt
  Wattage = "18 Watt";
  digitalWrite(one, nON);
  digitalWrite(two, OFF);
  digitalWrite(three, OFF);
  digitalWrite(four, OFF);
  digitalWrite(five, OFF);
  Serial.println(Wattage);
  Ignitor();
}
void SECOND() {                            // 36 Watt
  Wattage = "36 Watt";
  digitalWrite(one, nOFF);
  digitalWrite(two, ON);
  digitalWrite(three, OFF);
  digitalWrite(four, OFF);
  digitalWrite(five, OFF);
  Serial.println(Wattage);
  Ignitor();
}
void THIRD()  {                            // 54 Watt
  Wattage = "54 Watt";
  digitalWrite(one, nON);
  digitalWrite(two, ON);
  digitalWrite(three, OFF);
  digitalWrite(four, OFF);
  digitalWrite(five, OFF);
  Serial.println(Wattage);
  Ignitor();
}
void FOURTH()  {                           // 72 Watt
  Wattage = "72 Watt";
  digitalWrite(one, nOFF);
  digitalWrite(two, ON);
  digitalWrite(three, ON);
  digitalWrite(four, OFF);
  digitalWrite(five, OFF);
  Serial.println(Wattage);
  Ignitor();
}
void FIFTH()   {                           // 90 Watt
  Wattage = "90 Watt";
  digitalWrite(one, nON);
  digitalWrite(two, ON);
  digitalWrite(three, ON);
  digitalWrite(four, OFF);
  digitalWrite(five, OFF);
  Serial.println(Wattage);
  Ignitor();
  
}
void SIXTH()   {                           // 108 Watt
  Wattage = "108 Watt";
  digitalWrite(one, nOFF);
  digitalWrite(two, ON);
  digitalWrite(three, ON);
  digitalWrite(four, ON);
  digitalWrite(five, OFF);
  Serial.println(Wattage);
  Ignitor();
}
void SEVENTH() {                           // 126 Watt
  Wattage = "126 Watt";
  digitalWrite(one, nON);
  digitalWrite(two, ON);
  digitalWrite(three, ON);
  digitalWrite(four, ON);
  digitalWrite(five, OFF);
  Serial.println(Wattage);
  Ignitor();
}
void EIGHTH()  {                           // 144 Watt
  Wattage = "144 Watt";
  digitalWrite(one, nOFF);
  digitalWrite(two, ON);
  digitalWrite(three, ON);
  digitalWrite(four, ON);
  digitalWrite(five, ON);
  Serial.println(Wattage);
  Ignitor();
}
void NINTH()   {                           // 162 Watt
  Wattage = "162 Watt";
  digitalWrite(one, nON);
  digitalWrite(two, ON);
  digitalWrite(three, ON);
  digitalWrite(four, ON);
  digitalWrite(five, ON);
  Serial.println(Wattage);
  Ignitor();
}

//Indicator Color
void Red(){
  analogWrite(red, 90);
  digitalWrite(green, LOW);
  digitalWrite(blue, LOW);
}

void Yellow(){
  analogWrite(red, 90);
  analogWrite(green, 90);
  digitalWrite(blue, LOW);
}

void Green(){
  digitalWrite(red, LOW);
  analogWrite(green, 90);
  digitalWrite(blue, LOW);
}

void Cyan(){
  digitalWrite(red, LOW);
  analogWrite(green, 90);
  analogWrite(blue, 90);
}

void Blue(){
  digitalWrite(red, LOW);
  digitalWrite(green, LOW);
  analogWrite(blue, 90);
}

void Magenta(){
  analogWrite(red, 90);
  digitalWrite(green, LOW);
  analogWrite(blue, 90);
}

void White(){
  analogWrite(red, 90);
  analogWrite(green, 90);
  analogWrite(blue, 90);
}

///////////////////////////////////////////////////////////////  Light Sensor  ///////////////////////////////////////////////////////////////////

void LightSensor() {
  Serial.println("Photoresistor Mode");
  beep = 2;
   
  for (i = 0 ; WiFi.status() != WL_CONNECTED; ) {
    Photoresistor();
    delay(DELAY);
  }

}
///////////////////////////////////////////////////////////////  Automatic  ///////////////////////////////////////////////////////////////////

void Auto() { ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  time_t now = time(nullptr);
  struct tm* p_tm = localtime(&now);

  if ((p_tm->tm_hour == 0)) {                           /////////////////////////////////////////////    12 AM

    SEVENTH();

  } else if ((p_tm->tm_hour >= 1) && (p_tm->tm_hour <= 4)) {        /////////////////////////////////////////////    1 AM - 4 AM

    FIFTH();


  } else if ((p_tm->tm_hour == 5)) {                     /////////////////////////////////////////////   5 AM

    if ((p_tm->tm_min <= 29)) {                          //05:00:00 - 05:29:59
      FOURTH();

    } else if ((p_tm->tm_min >= 30)) {                    //05:30:00 - 05:59:59
      THIRD();
    }

  } else if ((p_tm->tm_hour == 6)) {                     /////////////////////////////////////////////   6 AM

    if ((p_tm->tm_min <= 29)) {                          //06:00:00 - 06:29:59
      FOURTH();

    } else if ((p_tm->tm_min >= 30)) {                    //06:30:00 - 06:59:59
      SHUT();
    }
  } else if ((p_tm->tm_hour >= 7) && (p_tm->tm_hour <= 16)) {               /////////////////////////////////////////////    7 AM - 4 PM

    SHUT();                                     //07:00:00 - 16:59:59

  } else if ((p_tm->tm_hour == 17)) {                     /////////////////////////////////////////////   5 PM

    if ((p_tm->tm_min <= 29)) {                          //17:00:00 - 17:29:59
      SHUT();

    } else if ((p_tm->tm_min >= 30)) {                    //17:30:00 - 17:59:59
      THIRD();
    }
  } else if ((p_tm->tm_hour >= 18) && (p_tm->tm_hour <= 20)) {                     /////////////////////////////////////////////   6 PM - 8 PM

    FOURTH();

  } else if ((p_tm->tm_hour >= 21) && (p_tm->tm_hour <= 23)) {                     /////////////////////////////////////////////   9 PM - 11 PM

    NINTH();

  }

}


void Photoresistor() { ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  int Photores = analogRead(Photo);
  if (Photores <= A ) {                     // 126 Watt state
    SEVENTH();

  } else if ( Photores > A + Dif && Photores <= B ) {                   // 90 Watt state
    FIFTH();

  } else if (Photores > B + Dif && Photores <= C ) {                   // 54 Watt state
    THIRD();

  } else if (Photores > C + Dif) {                                      //Off state
    SHUT();

  }
}

void Ignitor() { ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 if (ignState == 1){
    digitalWrite(ignPin, nON);
 }else if (ignState == 0){ 
    digitalWrite(ignPin, nOFF);
 }    
}

void handleNewMessages(int numNewMessages)
{
  WiFiClient client = server.available();
  Serial.print("handleNewMessages ");

  for (int i = 0; i < numNewMessages; i++)
  {
    String text = bot.messages[i].text;


    if (text == "/AutoReset")
    {
      if(Photostate == 1){  
        bot.sendMessage(CHAT_ID, "Street Light is automaticaly reset", "");
        Photostate = 0;
        EEPROM.write(1,0);
        EEPROM.commit();
      }else if(Photostate == 0){  
        bot.sendMessage(CHAT_ID, "Street Light is always on static", "");
        Photostate = 1;
        EEPROM.write(1,1);
        EEPROM.commit();
      }     
    }

    if (text == "/Ignitor")
    {
      if(ignState == 1){  
        bot.sendMessage(CHAT_ID, "Ignitor is disabled", "");
        ignState = 0;
        EEPROM.write(3,0);
        EEPROM.commit();
      }else if(ignState == 0){  
        bot.sendMessage(CHAT_ID, "Ignitor is enabled", "");
        ignState = 1;
        EEPROM.write(3,1);
        EEPROM.commit();
      }
      switch (Status) { //Static state refresh
        case 2:
          FIRST();
          break;
        case 3:
          SECOND();
          break;
        case 4:
          THIRD();
          break;
        case 5:
          FOURTH();
          break;
        case 6:
          FIFTH();
          break;
        case 7:
          SIXTH();
          break;
        case 8:
          SEVENTH();
          break;
        case 9:
          NINTH();
          break;
      }  
    }

    if (text == "/OFF")
    {
      bot.sendMessage(CHAT_ID, "Street Light is turned off", "");
      String request = "/OFF";
      SHUT();
      EEPROM.write(0,0);
      EEPROM.commit(); 
      Blue();
      Status = 1;
    }

    if (text == "/18Watt")
    {
      bot.sendMessage(CHAT_ID, "Current power of Street Light is 18 Watt", "");
      String request = "/18Watt";
      FIRST();
      EEPROM.write(0,0);
      EEPROM.commit(); 
      Yellow();
      Status = 2;
    }
    if (text == "/36Watt")
    {
      bot.sendMessage(CHAT_ID, "Current power of Street Light is 36 Watt", "");
      String request = "/36Watt";
      SECOND();
      EEPROM.write(0,0);
      EEPROM.commit();
      Yellow();
      Status = 3;
    }
    if (text == "/54Watt")
    {
      bot.sendMessage(CHAT_ID, "Current power of Street Light is 54 Watt", "");
      String request = "/54Watt";
      THIRD();
      EEPROM.write(0,0);
      EEPROM.commit();
      Yellow();
      Status = 4;
    }
    if (text == "/72Watt")
    {
      bot.sendMessage(CHAT_ID, "Current power of Street Light is 72 Watt", "");
      String request = "/72Watt";
      FOURTH();
      EEPROM.write(0,0);
      EEPROM.commit();
      Yellow();
      Status = 5;
    }
    if (text == "/90Watt")
    {
      bot.sendMessage(CHAT_ID, "Current power of Street Light is 90 Watt", "");
      String request = "/90Watt";
      FIFTH();
      EEPROM.write(0,0);
      EEPROM.commit();
      Yellow();
      Status = 6;
    }
    if (text == "/108Watt")
    {
      bot.sendMessage(CHAT_ID, "Current power of Street Light is 108 Watt", "");
      String request = "/108Watt";
      SIXTH();
      EEPROM.write(0,0);
      EEPROM.commit();
      Yellow();
      Status = 7;
    }
    if (text == "/126Watt")
    {
      bot.sendMessage(CHAT_ID, "Current power of Street Light is 126 Watt", "");
      String request = "/108Watt";
      SEVENTH();
      EEPROM.write(0,0);
      EEPROM.commit();
      Yellow();
      Status = 8;
    }
    if (text == "/162Watt")
    {
      bot.sendMessage(CHAT_ID, "Current power of Street Light is 162 Watt", "");
      String request = "/108Watt";
      NINTH();
      EEPROM.write(0,0);
      EEPROM.commit();
      White();
      Status = 9;
    }
    if (text == "/Photoresistor")
    {
      bot.sendMessage(CHAT_ID, "Current state of Street Light is in Photoresistor mode", "");
      Photoresistor();
      EEPROM.write(0,0);
      EEPROM.commit();
      Green();
      Status = 10;
      String request = "/Photoresistor";

    }
    if (text == "/Auto")
    {
      bot.sendMessage(CHAT_ID, "Current state of Street Light is in Automatic mode", "");
      Auto();
      EEPROM.write(0,1);
      EEPROM.commit();
      Cyan();
      Status = 11;
      String request = "/Auto";

    }
    if (text.equalsIgnoreCase("/status")) {
      switch (Status) {
        case 1:
          bot.sendMessage(CHAT_ID, "Street Light is turned off", "");
          break;
        case 2:
          bot.sendMessage(CHAT_ID, "Current power of Street Light is 18 Watt", "");
          break;
        case 3:
          bot.sendMessage(CHAT_ID, "Current power of Street Light is 36 Watt", "");
          break;
        case 4:
          bot.sendMessage(CHAT_ID, "Current power of Street Light is 54 Watt", "");
          break;
        case 5:
          bot.sendMessage(CHAT_ID, "Current power of Street Light is 72 Watt", "");
          break;
        case 6:
          bot.sendMessage(CHAT_ID, "Current power of Street Light is 90 Watt", "");
          break;
        case 7:
          bot.sendMessage(CHAT_ID, "Current power of Street Light is 108 Watt", "");
          break;
        case 8:
          bot.sendMessage(CHAT_ID, "Current power of Street Light is 126 Watt", "");
          break;
        case 9:
          bot.sendMessage(CHAT_ID, "Current power of Street Light is 162 Watt", "");
          break;
        case 10:
          bot.sendMessage(CHAT_ID, "Current state of Street Light is in Photoresistor mode: " + Wattage, "");
          break;
        case 11:
          bot.sendMessage(CHAT_ID, "Current state of Street Light is in Automatic mode: " + Wattage, "");
          break;
        default:
          bot.sendMessage(CHAT_ID, "Street Light is turned off", "");
      }
    }
    if (text.equalsIgnoreCase("/start"))
  {
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
// By Bahyyazid Ramadhan Hendarto - October 2019 & December 2023
