#include <DS3231.h>
DS3231 rtc(SDA,SCL);
Time t;


String DayOfWeek;
String Day;
String Hour;

int Received = 0;

int one = 6;       // 18 Watt Ballast
int two = 2;       // 36 Watt Ballast #1
int three = 3;     // 36 Watt Ballast #2
int four = 4;      // 36 Watt Ballast #3
int five = 5;      // 36 Watt Ballast #4
int Beeper = 7;    // Beeper
int Photo = A7;    // Photoresistor

int state = 0;

int i= 0;

int DELAY = 1000;

int beep = 0;

int A = 500;        // Between night and evening
int B = 2000;       // Between evening and dark skies
int C = 4000;   // Between dark skies and sunny daytime

bool OFF = LOW;
bool ON = HIGH;
bool nON = LOW;
bool nOFF = HIGH;


void setup() {    
//Serial.begin(9600);
//Serial.begin(57600);
Serial.begin(115200);
rtc.begin();

//remove comment to set the date and time
//rtc.setDOW(FRIDAY); rtc.setTime(23, 57, 0); rtc.setDate(1, 10, 2021);    

pinMode(A6,OUTPUT);       
pinMode(one,OUTPUT);
pinMode(two,OUTPUT);
pinMode(three,OUTPUT);
pinMode(four,OUTPUT);
pinMode(five,OUTPUT);
pinMode(Beeper,OUTPUT);

pinMode(Photo,INPUT);


digitalWrite(one,nOFF);
digitalWrite(two,OFF);
digitalWrite(three,OFF);
digitalWrite(four,OFF);
digitalWrite(five,OFF);

digitalWrite(A6,LOW);
beeping();

int Default = analogRead(A3);
if(Default >= 1000){
  Received = 'c';
  //Selection();
 }
}

void loop() {
  if(Serial.available() > 0 && Serial.available() != 13 && Serial.available() != 10 ){
  Received = Serial.read();
  Selection();
 }  
}  
   
void Selection(){
   switch (Received){

                 case 'a':                //OFF
                 
                 case '0':                //OFF (PC Only)
                 SHUT();
                 beep = 1;
                 beeping();
                 break;
                 
                 case '1':                //18 Watt
                 FIRST();
                 beep = 1;
                 beeping();
                 break;

                 case '2':                //36 Watt
                 SECOND();
                 beep = 1;
                 beeping();
                 break;

                 case '3':                //54 Watt
                 THIRD();
                 beep = 1;
                 beeping();
                 break;

                 case '4':                //72 Watt
                 FOURTH();
                 beep = 1;
                 beeping();
                 break;

                 case '5':                //90 Watt
                 FIFTH();
                 beep = 1;
                 beeping();
                 break;

                 case '6':                //108 Watt
                 SIXTH();
                 beep = 1;
                 beeping();
                 break;

                 case '7':                //126 Watt
                 SEVENTH();
                 beep = 1;
                 beeping();
                 break;

                 case '9':                //144 Watt
                 EIGHTH();
                 beep = 1;
                 beeping();
                 break;
                 
                 case '8':                //162 Watt
                 NINTH();
                 beep = 1;
                 beeping();
                 break;


                 case 'b':                //Light Sensor
                 LightSensor();
                 break;                 
                 
                 case 'c':                //Automatic  
                 RTC();
                 break;

                 case 'd':                //Motion Sensor
                 Motion();
                 break;

                 case 'g':
                 beep = 4;
                 beeping();
                 Serial.println("100W not available");
                 break;
                 
                 case 'h':
                 beep = 4;
                 beeping();
                 Serial.println("225W not available");
                 break;

                 case 'j':
                 beep = 4;
                 beeping();
                 Serial.println("375W not available");
                 break;

                 case 'k':
                 beep = 4;
                 beeping();
                 Serial.println("500W not available");
                 break;

                 case 'l':
                 beep = 4;
                 beeping();
                 Serial.println("675W not available");
                 break;
                 
  
 }
}

///////////////////////////////////////////////////////////////////MANUAL //////////////////////////////////////////////////////////////////////

void SHUT()   {                            // OFF
                 digitalWrite(one,nOFF);
                 digitalWrite(two,OFF);
                 digitalWrite(three,OFF);
                 digitalWrite(four,OFF);
                 digitalWrite(five,OFF);
                 Serial.println("OFF");
                 
              }
                 
void FIRST()  {                            // 18 Watt
                 digitalWrite(one,nON);
                 digitalWrite(two,OFF);
                 digitalWrite(three,OFF);
                 digitalWrite(four,OFF);
                 digitalWrite(five,OFF);
                 Serial.println("18 Watt");                   
              }
void SECOND(){                             // 36 Watt
                 digitalWrite(one,nOFF);
                 digitalWrite(two,ON);
                 digitalWrite(three,OFF);
                 digitalWrite(four,OFF);
                 digitalWrite(five,OFF);
                 Serial.println("36 Watt");                   
              }
void THIRD()  {                            // 54 Watt
                 digitalWrite(one,nON);
                 digitalWrite(two,ON);
                 digitalWrite(three,OFF);
                 digitalWrite(four,OFF);
                 digitalWrite(five,OFF);
                 Serial.println("54 Watt");                   
               }
void FOURTH()  {                           // 72 Watt
                 digitalWrite(one,nOFF);
                 digitalWrite(two,ON);
                 digitalWrite(three,ON);
                 digitalWrite(four,OFF);
                 digitalWrite(five,OFF);
                 Serial.println("72 Watt");                   
               }
void FIFTH()   {                           // 90 Watt
                 digitalWrite(one,nON);
                 digitalWrite(two,ON);
                 digitalWrite(three,ON);
                 digitalWrite(four,OFF);
                 digitalWrite(five,OFF);
                 Serial.println("90 Watt");                   
               }
void SIXTH()   {                           // 108 Watt
                 digitalWrite(one,nOFF);
                 digitalWrite(two,ON);
                 digitalWrite(three,ON);
                 digitalWrite(four,ON);
                 digitalWrite(five,OFF);
                 Serial.println("108 Watt");                   
               }                                                                      
void SEVENTH() {                           // 126 Watt
                 digitalWrite(one,nON);
                 digitalWrite(two,ON);
                 digitalWrite(three,ON);
                 digitalWrite(four,ON);
                 digitalWrite(five,OFF);
                 Serial.println("126 Watt");                   
               }
void EIGHTH()  {                           // 144 Watt
                 digitalWrite(one,nOFF);
                 digitalWrite(two,ON);
                 digitalWrite(three,ON);
                 digitalWrite(four,ON);
                 digitalWrite(five,ON);
                 Serial.println("144 Watt");                   
               }
void NINTH()   {                           // 162 Watt
                 digitalWrite(one,nON);
                 digitalWrite(two,ON);
                 digitalWrite(three,ON);
                 digitalWrite(four,ON);
                 digitalWrite(five,ON);
                 Serial.println("162 Watt");                   
               }
   
    
///////////////////////////////////////////////////////////////  Light Sensor  ///////////////////////////////////////////////////////////////////    

void LightSensor(){
                 Serial.println("Photoresistor Mode");
                 beep = 2;
                 beeping();
                 for(i=0 ; Received <=100 ; ){ 
                    if(Received == 98){
                    Photoresistor();
                    delay(DELAY);
                    Received = Serial.read(); 
                    continue;
                    }else if(Received == -1){
                    Photoresistor();
                    delay(DELAY);
                    Received = Serial.read(); 
                    continue;    
                    }else if(Received != 'b' && Received >= 0){
                    Serial.println("Photoresistor Mode Ended");
                    Selection();  
                    break;                        
                    }else{
                    delay(DELAY);
                    Received = Serial.read();
                    continue; 
               }
           }  
  
}
///////////////////////////////////////////////////////////////  Automatic  ///////////////////////////////////////////////////////////////////    

void RTC(){
                 Serial.println("RTC Mode");
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
                    Serial.println("RTC Mode Ended");
                    Selection();
                    break;  
                    }else{
                    Auto();
                    delay(DELAY);
                    Received = Serial.read();
                    continue;
                    }
                 }  
  
}
////////////////////////////////////////////////////////////////////////////  Motion Sensor  ///////////////////////////////////////////////////////////////////
void Motion(){
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
                    Serial.println("Motion Mode Ended");
                    Selection();
                    break;                        
                    }else{
                    delay(500);
                    Received = Serial.read();
                    continue; 
               }
           }  
  
} 
void Photoresistor(){ ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    
                 int Photores = analogRead(A7);
                 if(Photores >= 0 && Photores <= A ){                       // 126 Watt state
                 SEVENTH();
                      
                 }else if( Photores > A && Photores <=B ){                      // 90 Watt state
                 FIFTH();
                               
                 }else if(Photores > B && Photores <=C ){                      // 54 Watt state
                 THIRD();
                                  
                 }else if(Photores > C){                                        //Off state
                 SHUT();

                 }
                 Serial.println(Photores);
}               
void Auto(){ ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                 DayOfWeek = rtc.getDOWStr();
                 Day = rtc.getDateStr();
                 Hour = rtc.getTimeStr();
                 t = rtc.getTime();
                 
        if(t.hour == 00){                             /////////////////////////////////////////////    12 AM

                    if(t.min <=2){                             //00:00:00 - 00:02:59
                    SEVENTH(); 
                    
                    }else if(t.min >= 3){                   //00:03:00 - 00:05:59
                    MotionSensor();
                    } 

        }else if(t.hour >= 01 && t.hour <= 04){                 /////////////////////////////////////////////    1 AM - 4 AM

                    MotionSensor(); 
        
        }else if(t.hour == 05){                       /////////////////////////////////////////////   5 AM

                    if(t.min <=29){                             //05:00:00 - 05:29:59
                    MotionSensor(); 
                    
                    }else if(t.min >= 30){                       //05:30:00 - 05:59:59
                    THIRD();
                    }
        }else if(t.hour == 06){                       /////////////////////////////////////////////   6 AM             
                            
                    if(t.min <=29){                             //06:00:00 - 06:29:59
                    FOURTH();
                    
                    }else if(t.min >= 30){                       //06:30:00 - 06:59:59
                    SHUT();
                    }
        }else if(t.hour >= 07 && t.hour <= 16){                 /////////////////////////////////////////////    7 AM - 4 PM

                    SHUT();                                     //07:00:00 - 16:59:59

        }else if(t.hour == 17){                       /////////////////////////////////////////////   5 PM             
                            
                    if(t.min <=29){                             //17:00:00 - 17:29:59
                    SHUT();
                    
                    }else if(t.min > 30){                       //17:30:00 - 17:59:59
                    THIRD();
                    }                        
        }else if(t.hour >= 18 && t.hour <= 20){                       /////////////////////////////////////////////   6 PM - 8 PM            
                            
                    FOURTH();

        }else if(t.hour >= 21 && t.hour <= 23){                       /////////////////////////////////////////////   9 PM - 11 PM            
                            
                    NINTH();                                       

        } 
        Serial.println(DayOfWeek);
        Serial.println(Day);
        Serial.println(Hour);
}
void MotionSensor(){/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        int PIR = analogRead(A1); 
        
        if(PIR >=500){
        
        SIXTH(); 
        
        }else{ 
        
        FOURTH();
 }
 Serial.println(PIR);
}

void beeping(){/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  if(beep == 0){                //long single
     digitalWrite(Beeper,ON);
     delay(200);
     digitalWrite(Beeper,OFF);
     delay(50);

  }else if(beep == 1){         //single
     digitalWrite(Beeper,ON);
     delay(50);
     digitalWrite(Beeper,OFF);
     delay(50);
         
  }else if(beep == 2){         //double
     digitalWrite(Beeper,ON);
     delay(50);
     digitalWrite(Beeper,OFF);
     delay(50);
     digitalWrite(Beeper,ON);
     delay(50);
     digitalWrite(Beeper,OFF);
     delay(50);  
  
  }else if(beep == 3){         //triple
     digitalWrite(Beeper,ON);
     delay(50);
     digitalWrite(Beeper,OFF);
     delay(50);
     digitalWrite(Beeper,ON);
     delay(50);
     digitalWrite(Beeper,OFF);
     delay(50);
     digitalWrite(Beeper,ON);
     delay(50);
     digitalWrite(Beeper,OFF);
     delay(50);
   }else if(beep == 4){         //Fail
     digitalWrite(Beeper,ON);
     delay(1000);
     digitalWrite(Beeper,OFF);
   }       
     
}


// By Bahyyazid Ramadhan Hendarto - October 2019
