#include <ESP8266WiFi.h>
#include <time.h>
#include <UniversalTelegramBot.h>

const char* ssid = "TK AL-YAZID";//your-ssid
const char* password = "abyazid1";//your-password

const char* ssidBackup = "Bahyyazid";//your-secondary-ssid
const char* passwordBackup = "bahyyazid1234";//your-secondary-password

WiFiServer server(80);

#define BOT_TOKEN "6954813387:AAEKT8dJ5voY8VDdoRebMxWyXwRvfmznyDs"
#define CHAT_ID "1875645986"
const unsigned long BOT_MTBS = 1000; // mean time between scan messages

X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
unsigned long bot_lasttime; // last time messages' scan has been done




int Status = 0;
int i = 0;
int DELAY = 1000;

int beep = 0;


int timezone = 7 * 3600;
int dst = 0;

bool OFF = LOW;
bool ON = HIGH;
bool nON = LOW;
bool nOFF = HIGH;

String Wattage;

int one = D1;       // 18 Watt Ballast
int two = D2;       // 36 Watt Ballast #1
int three = D3;     // 36 Watt Ballast #2
int four = D4;      // 36 Watt Ballast #3
int five = D6;      // 36 Watt Ballast #4
int Beeper = D7;    // Beeper
int Photo = A0;    // Photoresistor

int A = 500;        // Between night and evening
int B = 725;       // Between evening and dark skies
int C = 950;   // Between dark skies and sunny daytime


void setup()
{
  Serial.begin(115200);
  pinMode(Photo, INPUT);
  pinMode(one, OUTPUT);
  pinMode(two, OUTPUT);
  pinMode(three, OUTPUT);
  pinMode(four, OUTPUT);
  pinMode(five, OUTPUT);
  pinMode(Beeper, OUTPUT);

  digitalWrite(one, nOFF);
  digitalWrite(two, OFF);
  digitalWrite(three, OFF);
  digitalWrite(four, OFF);
  digitalWrite(five, OFF);



  Serial.print("Connecting");
  WiFi.begin(ssid, password);
  for (i <= 20; WiFi.status() != WL_CONNECTED ; i++) {
    if (i < 20) {
      delay(500);
      Serial.print(".");
    } else if (i == 20) {
      Serial.println("Connecting to Wifi Failed, looking for mobile hotspot");
      WiFi.begin(ssidBackup, passwordBackup);
    } else if (i > 20 && i < 40) {
      delay(500);
      Serial.print(".");
    } else if (i == 40) {
      Serial.println("Connecting to Wifi Failed again, restarting module");
    }
  }
  i = 0;
  Serial.println("WiFi connected");
  server.begin();  // Starts the Server
  Serial.println("Server started");

  Serial.print("IP Address of network: "); // Prints IP address on Serial Monitor
  Serial.println(WiFi.localIP().toString());
  Serial.print("Copy and paste the following URL: https://");
  Serial.print(WiFi.localIP().toString());
  Serial.println("/");

  Serial.println("\nWaiting for Internet time");

  while (!time(nullptr)) {
    Serial.print("*");
    delay(1000);
  }
  Serial.println("\nTime response....OK");
  configTime(7 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  secured_client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org
}


void loop() {
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
    Status = 1;
  }
  if (request.indexOf("/18Watt") != -1) {
    FIRST();
    Status = 2;
  }
  if (request.indexOf("/36Watt") != -1) {
    SECOND();
    Status = 3;
  }
  if (request.indexOf("/54Watt") != -1) {
    THIRD();
    Status = 4;
  }
  if (request.indexOf("/72Watt") != -1) {
    FOURTH();
    Status = 5;
  }
  if (request.indexOf("/90Watt") != -1) {
    FIFTH();
    Status = 6;
  }
  if (request.indexOf("/108Watt") != -1) {
    SIXTH();
    Status = 7;
  }
  if (request.indexOf("/126Watt") != -1) {
    SEVENTH();
    Status = 8;
  }
  if (request.indexOf("/162Watt") != -1) {
    NINTH();
    Status = 9;
  }
  if (request.indexOf("/Photoresistor") != -1) {
    Status = 10;

  }
  if (request.indexOf("/Auto") != -1) {
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
    client.println("<ul><a href=\"/OFF\"\"><form class=\"OFF\"><img src=\"https://i.ibb.co/JcV8df8/OFFPressed.png\" alt=\"\" height=\"80px\" border=\"none\"></button></a></ul>");
  } else {
    client.println("<ul><a href=\"/OFF\"\"><form class=\"OFF\"><img src=\"https://i.ibb.co/6H29kTh/OFF.png\" alt=\"\" height=\"80px\" border=\"none\"></button></a></ul>");
  } if (Status == 2) {
    client.println("<ul><a href=\"/18Watt\"\"><form class=\"18Watt\"><img src=\"https://i.ibb.co/HqH6KhM/18-WPressed.png\" alt=\"\" height=\"80px\"></button></a></ul>");
  } else {
    client.println("<ul><a href=\"/18Watt\"\"><form class=\"18Watt\"><img src=\"https://i.ibb.co/mRZCx3x/18W.png\" alt=\"\" height=\"80px\"></button></a></ul>");
  } if (Status == 3) {
    client.println("<ul><a href=\"/36Watt\"\"><form class=\"36Watt\"><img src=\"https://i.ibb.co/rHk9mVJ/36-WPressed.png\" alt=\"\" height=\"80px\"></button></a></ul>");
  } else {
    client.println("<ul><a href=\"/36Watt\"\"><form class=\"36Watt\"><img src=\"https://i.ibb.co/cT3SL6d/36W.png\" alt=\"\" height=\"80px\"></button></a></ul>");
  } if (Status == 4) {
    client.println("<ul><a href=\"/54Watt\"\"><form class=\"54Watt\"><img src=\"https://i.ibb.co/kcdy2ks/54-WPressed.png\" alt=\"\" height=\"80px\"></button></a></ul>");
  } else {
    client.println("<ul><a href=\"/54Watt\"\"><form class=\"54Watt\"><img src=\"https://i.ibb.co/QPtzXPz/54W.png\" alt=\"\" height=\"80px\"></button></a></ul>");
  } if (Status == 5) {
    client.println("<ul><a href=\"/72Watt\"\"><form class=\"72Watt\"><img src=\"https://i.ibb.co/rmmMnKK/72-WPressed.png\"alt=\"\" height=\"80px\"></button></a></ul>");
  } else {
    client.println("<ul><a href=\"/72Watt\"\"><form class=\"72Watt\"><img src=\"https://i.ibb.co/Y0Mp2Gk/72W.png\"alt=\"\" height=\"80px\"></button></a></ul>");
  } if (Status == 6) {
    client.println("<ul><a href=\"/90Watt\"\"><form class=\"90Watt\"><img src=\"https://i.ibb.co/dcN0zRC/90-WPressed.png\" alt=\"\" height=\"80px\"></button></a></ul>");
  } else {
    client.println("<ul><a href=\"/90Watt\"\"><form class=\"90Watt\"><img src=\"https://i.ibb.co/DMDHjk3/90W.png\" alt=\"\" height=\"80px\"></button></a></ul>");
  } if (Status == 7) {
    client.println("<ul><a href=\"/108Watt\"\"><form class=\"108Watt\"><img src=\"https://i.ibb.co/Q6ZgPbs/108-WPressed.png\" alt=\"\" height=\"80px\"></button></a></ul>");
  } else {
    client.println("<ul><a href=\"/108Watt\"\"><form class=\"108Watt\"><img src=\"https://i.ibb.co/Xp8bcyH/108W.png\" alt=\"\" height=\"80px\"></button></a></ul>");
  } if (Status == 8) {
    client.println("<ul><a href=\"/126Watt\"\"><form class=\"126Watt\"><img src=\"https://i.ibb.co/H7s3yt0/126-WPressed.png\" alt=\"\" height=\"80px\"></button></a></ul>");
  } else {
    client.println("<ul><a href=\"/126Watt\"\"><form class=\"126Watt\"><img src=\"https://i.ibb.co/fMX2QHd/126W.png\" alt=\"\" height=\"80px\"></button></a></ul>");
  } if (Status == 9) {
    client.println("<ul><a href=\"/162Watt\"\"><form class=\"162Watt\"><img src=\"https://i.ibb.co/PFw8FwH/162-WPressed.png\" alt=\"\" height=\"80px\"></button></a></ul>");
  } else {
    client.println("<ul><a href=\"/162Watt\"\"><form class=\"162Watt\"><img src=\"https://i.ibb.co/Z1TpgV9/162W.png\" alt=\"\" height=\"80px\"></button></a></ul>");
  } if (Status == 10) {
    client.println("<ul><a href=\"/Photoresistor\"\"><form class=\"Photoresistor\"><img src=\"https://i.ibb.co/k0g0cQP/Photocell-Activated.png\" alt=\"\" height=\"80px\"></button></a></ul>");
    Photoresistor();
    client.println("<script>");
    client.println("function autoRefresh() {");
    client.println("window.location = window.location.href;");
    client.println("}");
    client.println("setInterval('autoRefresh()', 1000);");
    client.println("</script>");
  } else {
    client.println("<ul><a href=\"/Photoresistor\"\"><form class=\"Photoresistor\"><img src=\"https://i.ibb.co/VmxgZJQ/Photocell.png\" alt=\"\" height=\"80px\"></button></a></ul>");
  } if (Status == 11) {
    client.println("<ul><a href=\"/Auto\"\"><form class=\"Auto\"><img src=\"https://i.ibb.co/2FCZLfw/Auto-Pressed.png\" alt=\"\" height=\"80px\"></form></a></ul>");
    Auto();
    client.println("<script>");
    client.println("function autoRefresh() {");
    client.println("window.location = window.location.href;");
    client.println("}");
    client.println("setInterval('autoRefresh()', 1000);");
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
///////////////////////////////////////////////////////////////////MANUAL //////////////////////////////////////////////////////////////////////

void SHUT()   {                            // OFF
  Wattage = "OFF";
  digitalWrite(one, nOFF);
  digitalWrite(two, OFF);
  digitalWrite(three, OFF);
  digitalWrite(four, OFF);
  digitalWrite(five, OFF);
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
}
void SECOND() {                            // 36 Watt
  Wattage = "36 Watt";
  digitalWrite(one, nOFF);
  digitalWrite(two, ON);
  digitalWrite(three, OFF);
  digitalWrite(four, OFF);
  digitalWrite(five, OFF);
  Serial.println(Wattage);
}
void THIRD()  {                            // 54 Watt
  Wattage = "54 Watt";
  digitalWrite(one, nON);
  digitalWrite(two, ON);
  digitalWrite(three, OFF);
  digitalWrite(four, OFF);
  digitalWrite(five, OFF);
  Serial.println(Wattage);
}
void FOURTH()  {                           // 72 Watt
  Wattage = "72 Watt";
  digitalWrite(one, nOFF);
  digitalWrite(two, ON);
  digitalWrite(three, ON);
  digitalWrite(four, OFF);
  digitalWrite(five, OFF);
  Serial.println(Wattage);
}
void FIFTH()   {                           // 90 Watt
  Wattage = "90 Watt";
  digitalWrite(one, nON);
  digitalWrite(two, ON);
  digitalWrite(three, ON);
  digitalWrite(four, OFF);
  digitalWrite(five, OFF);
  Serial.println(Wattage);
}
void SIXTH()   {                           // 108 Watt
  Wattage = "108 Watt";
  digitalWrite(one, nOFF);
  digitalWrite(two, ON);
  digitalWrite(three, ON);
  digitalWrite(four, ON);
  digitalWrite(five, OFF);
  Serial.println(Wattage);
}
void SEVENTH() {                           // 126 Watt
  Wattage = "126 Watt";
  digitalWrite(one, nON);
  digitalWrite(two, ON);
  digitalWrite(three, ON);
  digitalWrite(four, ON);
  digitalWrite(five, OFF);
  Serial.println(Wattage);
}
void EIGHTH()  {                           // 144 Watt
  Wattage = "144 Watt";
  digitalWrite(one, nOFF);
  digitalWrite(two, ON);
  digitalWrite(three, ON);
  digitalWrite(four, ON);
  digitalWrite(five, ON);
  Serial.println(Wattage);
}
void NINTH()   {                           // 162 Watt
  Wattage = "162 Watt";
  digitalWrite(one, nON);
  digitalWrite(two, ON);
  digitalWrite(three, ON);
  digitalWrite(four, ON);
  digitalWrite(five, ON);
  Serial.println(Wattage);
}


///////////////////////////////////////////////////////////////  Light Sensor  ///////////////////////////////////////////////////////////////////

void LightSensor() {
  Serial.println("Photoresistor Mode");
  beep = 2;
  beeping();
  for (i = 0 ; WiFi.status() != WL_CONNECTED; ) {
    Photoresistor();
    delay(DELAY);
  }

}
///////////////////////////////////////////////////////////////  Automatic  ///////////////////////////////////////////////////////////////////

void RTC() {
  /*                 Serial.println("RTC Mode");
                   beep = 3;
                   beeping();
                   for(i=0 ; Received < 100 ; ){
                      if(Received == 99){
                      Auto();
                      delay(DELAY);
                      Received = Serial.read();
                      continue;
                      }else if(Received == -1){
                      Auto();
                      delay(DELAY);
                      Received = Serial.read();
                      continue;
                      }else if(Received != 'c' && Received >= 0){
                        if( Received != 10 && Received != 13){
                          Serial.println("RTC Mode Ended");
                          Selection();
                          break;
                        }else{
                          Auto();
                          delay(DELAY);
                          Received = Serial.read();
                          continue;
                        }
                      }else{
                      Auto();
                      delay(DELAY);
                      Received = Serial.read();
                      continue;
                      }
                   }
  */
}

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
////////////////////////////////////////////////////////////////////////////  Motion Sensor  ///////////////////////////////////////////////////////////////////
void Motion() {
  /*
                   Serial.println("Motion Mode");
                   beep = 0;
                   beeping();
                   beeping();
                   for(i=0 ; Received <=100 ; ){
                      if(Received == 98){
                      MotionSensor();
                      delay(500);
                      Received = Serial.read();
                      continue;
                      }else if(Received == -1){
                      MotionSensor();
                      delay(500);
                      Received = Serial.read();
                      continue;
                      }else if(Received != 'd' && Received >= 0){
                        if( Received != 10 && Received != 13){
                          Serial.println("Motion Mode Ended");
                          Selection();
                          break;
                        }else{
                          Motion();
                          Received = Serial.read();
                          continue;
                        }
                      }else{
                      delay(500);
                      Received = Serial.read();
                      continue;
                 }
             }

  */
}

void Photoresistor() { ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  int Photores = analogRead(Photo);
  if (Photores >= 0 && Photores <= A ) {                     // 126 Watt state
    SEVENTH();

  } else if ( Photores > A && Photores <= B ) {                   // 90 Watt state
    FIFTH();

  } else if (Photores > B && Photores <= C ) {                   // 54 Watt state
    THIRD();

  } else if (Photores > C) {                                      //Off state
    SHUT();

  }
}



void beeping() { /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (beep == 0) {              //long single
    digitalWrite(Beeper, ON);
    delay(200);
    digitalWrite(Beeper, OFF);
    delay(50);

  } else if (beep == 1) {       //single
    digitalWrite(Beeper, ON);
    delay(50);
    digitalWrite(Beeper, OFF);
    delay(50);

  } else if (beep == 2) {       //double
    digitalWrite(Beeper, ON);
    delay(50);
    digitalWrite(Beeper, OFF);
    delay(50);
    digitalWrite(Beeper, ON);
    delay(50);
    digitalWrite(Beeper, OFF);
    delay(50);

  } else if (beep == 3) {       //triple
    digitalWrite(Beeper, ON);
    delay(50);
    digitalWrite(Beeper, OFF);
    delay(50);
    digitalWrite(Beeper, ON);
    delay(50);
    digitalWrite(Beeper, OFF);
    delay(50);
    digitalWrite(Beeper, ON);
    delay(50);
    digitalWrite(Beeper, OFF);
    delay(50);
  } else if (beep == 4) {       //Fail
    digitalWrite(Beeper, ON);
    delay(1000);
    digitalWrite(Beeper, OFF);
  }

}

void handleNewMessages(int numNewMessages)
{
  WiFiClient client = server.available();
  Serial.print("handleNewMessages ");

  for (int i = 0; i < numNewMessages; i++)
  {
    String text = bot.messages[i].text;


    if (text == "/OFF")
    {
      bot.sendMessage(CHAT_ID, "Street Light is turned off", "");
      String request = "/OFF";
      SHUT();
      Status = 1;
    }

    if (text == "/18Watt")
    {
      bot.sendMessage(CHAT_ID, "Current power of Street Light is 18 Watt", "");
      String request = "/18Watt";
      FIRST();
      Status = 2;
    }
    if (text == "/36Watt")
    {
      bot.sendMessage(CHAT_ID, "Current power of Street Light is 36 Watt", "");
      String request = "/36Watt";
      SECOND();
      Status = 3;
    }
    if (text == "/54Watt")
    {
      bot.sendMessage(CHAT_ID, "Current power of Street Light is 54 Watt", "");
      String request = "/54Watt";
      THIRD();
      Status = 4;
    }
    if (text == "/72Watt")
    {
      bot.sendMessage(CHAT_ID, "Current power of Street Light is 72 Watt", "");
      String request = "/72Watt";
      FOURTH();
      Status = 5;
    }
    if (text == "/90Watt")
    {
      bot.sendMessage(CHAT_ID, "Current power of Street Light is 90 Watt", "");
      String request = "/90Watt";
      FIFTH();
      Status = 6;
    }
    if (text == "/108Watt")
    {
      bot.sendMessage(CHAT_ID, "Current power of Street Light is 108 Watt", "");
      String request = "/108Watt";
      SIXTH();
      Status = 7;
    }
    if (text == "/126Watt")
    {
      bot.sendMessage(CHAT_ID, "Current power of Street Light is 126 Watt", "");
      String request = "/108Watt";
      SEVENTH();
      Status = 8;
    }
    if (text == "/162Watt")
    {
      bot.sendMessage(CHAT_ID, "Current power of Street Light is 162 Watt", "");
      String request = "/108Watt";
      NINTH();
      Status = 9;
    }
    if (text == "/Photoresistor")
    {
      bot.sendMessage(CHAT_ID, "Current state of Street Light is in Photoresistor mode", "");
      Photoresistor();
      Status = 10;
      String request = "/Photoresistor";

    }
    if (text == "/Auto")
    {
      bot.sendMessage(CHAT_ID, "Current state of Street Light is in Automatic mode", "");
      Auto();
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
      bot.sendMessage(CHAT_ID, welcome, "Markdown");
    }
  }
}
// By Bahyyazid Ramadhan Hendarto - October 2019 & December 2023
