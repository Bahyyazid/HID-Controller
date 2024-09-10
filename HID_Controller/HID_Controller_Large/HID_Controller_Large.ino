#include <DS3231.h>
DS3231 rtc(SDA,SCL);
Time t;


String DayOfWeek;
String Day;
String Hour;

int Received = 0;

int one = 2;      // 100 Watt Ballast
int two = 3;       // 125 Watt Ballast
int three = 4;     // 250 Watt Ballast
int four = 5;      // 400 Watt Ballast

int Beeper = 7;    // Beeper
int Photo = A7;    // Photoresistor


int i= 0;

int DELAY = 1000;

int beep = 0;

int A = 40;        // Between night and evening
int B = 120;       // Between evening and dark skies
int C = 350;   // Between dark skies and sunny daytime

bool OFF = LOW;
bool ON = HIGH;


void setup() {    
Serial.begin(115200);
rtc.begin();

//remove comment to set the date and time
//rtc.setDOW(FRIDAY); rtc.setTime(23, 57, 0); rtc.setDate(1, 10, 2021);    

pinMode(one,OUTPUT);       
pinMode(two,OUTPUT);
pinMode(three,OUTPUT);
pinMode(four,OUTPUT);
pinMode(Beeper,OUTPUT);

pinMode(Photo,INPUT);


digitalWrite(one,OFF);
digitalWrite(two,OFF);
digitalWrite(three,OFF);
digitalWrite(four,OFF);
beeping();

int Default = analogRead(A3);
if(Default >= 1000){
  Received = 'c';
  Selection();
 }
}

void loop() {
  if(Serial.available() > 0){
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
                 beep = 4;
                 beeping();
                 Serial.println("18W not available");
                 break;

                 case '2':                //36 Watt
                 beep = 4;
                 beeping();
                 Serial.println("36W not available");
                 break;

                 case '3':                //54 Watt
                 beep = 4;
                 beeping();
                 Serial.println("54W not available");
                 break;

                 case '4':                //72 Watt
                 beep = 4;
                 beeping();
                 Serial.println("72W not available");
                 break;

                 case '5':                //90 Watt
                 beep = 4;
                 beeping();
                 Serial.println("90W not available");
                 break;

                 case '6':                //108 Watt
                 beep = 4;
                 beeping();
                 Serial.println("108W not available");
                 break;

                 case '7':                //126 Watt
                 beep = 4;
                 beeping();
                 Serial.println("126W not available");
                 break;

                 case '9':                //144 Watt
                 beep = 4;
                 beeping();
                 Serial.println("144W not available");
                 break;
                 
                 case '8':                //162 Watt
                 beep = 4;
                 beeping();
                 Serial.println("162W not available");
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

                 case 'g':                //100 Watt
                 beep = 1;
                 FIRST();
                 beeping();
                 Serial.println("100 Watt");
                 break;
                 
                 case 'h':                //225 Watt
                 beep = 1;
                 SECOND();
                 beeping();
                 Serial.println("225 Watt");
                 break;

                 case 'j':                //350 Watt
                 beep = 1;
                 THIRD();
                 beeping();
                 Serial.println("350 Watt");
                 break;

                 case 'k':                //500 Watt
                 beep = 1;
                 FOURTH();
                 beeping();
                 Serial.println("500 Watt");
                 break;

                 case 'l':                //625 Watt
                 beep = 1;
                 FIFTH();
                 beeping();
                 Serial.println("625 Watt");
                 break;
                 
                 case 'm':
                 beep = 1;
                 SIXTH();
                 beeping();
                 Serial.println("250 Watt");
                 break;

                 case 'n':
                 beep = 1;
                 SEVENTH();
                 beeping();
                 Serial.println("400 Watt");
                 break;
  
 }
}

///////////////////////////////////////////////////////////////////MANUAL //////////////////////////////////////////////////////////////////////

void SHUT()   {                            // OFF
                 digitalWrite(one,OFF);
                 digitalWrite(two,OFF);
                 digitalWrite(three,OFF);
                 digitalWrite(four,OFF);
                 Serial.println("OFF");
                 
              }
                 
void FIRST()  {                            // 100 Watt
                 digitalWrite(one,ON);
                 digitalWrite(two,OFF);
                 digitalWrite(three,OFF);
                 digitalWrite(four,OFF);
                 Serial.println("100 Watt");                   
              }
void SECOND(){                             // 225 Watt
                 digitalWrite(one,ON);
                 digitalWrite(two,ON);
                 digitalWrite(three,OFF);
                 digitalWrite(four,OFF);
                 Serial.println("225 Watt");                   
              }
void THIRD()  {                            // 350 Watt
                 digitalWrite(one,ON);
                 digitalWrite(two,OFF);
                 digitalWrite(three,ON);
                 digitalWrite(four,OFF);
                 Serial.println("375 Watt");                  
               }
void FOURTH()  {                           // 500 Watt
                 digitalWrite(one,ON);
                 digitalWrite(two,OFF);
                 digitalWrite(three,ON);
                 digitalWrite(four,ON);
                 Serial.println("500 Watt");                   
               }
void FIFTH()   {                           // 625 Watt
                 digitalWrite(one,ON);
                 digitalWrite(two,ON);
                 digitalWrite(three,ON);
                 digitalWrite(four,ON);
                 Serial.println("625 Watt");                   
               }
void SIXTH()   {                           // 250 Watt    Mercury lamps only
                 digitalWrite(one,OFF);
                 digitalWrite(two,OFF);
                 digitalWrite(three,ON);
                 digitalWrite(four,OFF);
                 Serial.println("250 Watt"); 
                 }
void SEVENTH()   {                        // 400 Watt    Mercury lamps only
                 digitalWrite(one,OFF);
                 digitalWrite(two,OFF);
                 digitalWrite(three,ON);
                 digitalWrite(four,ON);
                 Serial.println("400 Watt");                                       
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
                    Selection();
                    Serial.println("Photoresistor Mode Ended");
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
                 if(Photores >= 0 && Photores <= A ){                       // 350 Watt state
                 THIRD();
                      
                 }else if( Photores > A && Photores <=B ){                      // 225 Watt state
                 SECOND();
                               
                 }else if(Photores > B && Photores <=C ){                      // 100 Watt state
                 FIRST();
                                  
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
                    FIRST(); 
                    
                    }else if(t.min >= 3){                   //00:03:00 - 00:05:59
                    MotionSensor();
                    } 

        }else if(t.hour >= 01 && t.hour <= 04){                 /////////////////////////////////////////////    1 AM - 4 AM

                    MotionSensor(); 
        
        }else if(t.hour == 05){                       /////////////////////////////////////////////   5 AM

                    if(t.min <=29){                             //05:00:00 - 05:29:59
                    MotionSensor(); 
                    
                    }else if(t.min >= 30){                       //05:30:00 - 05:59:59
                    FIRST();
                    }
        }else if(t.hour == 06){                       /////////////////////////////////////////////   6 AM             
                            
                    if(t.min <=29){                             //06:00:00 - 06:29:59
                    FIRST();
                    
                    }else if(t.min >= 30){                       //06:30:00 - 06:59:59
                    SHUT();
                    }
        }else if(t.hour >= 07 && t.hour <= 16){                 /////////////////////////////////////////////    7 AM - 4 PM

                    SHUT();                                     //07:00:00 - 16:59:59

        }else if(t.hour == 17){                       /////////////////////////////////////////////   5 PM             
                            
                    if(t.min <=29){                             //17:00:00 - 17:29:59
                    SHUT();
                    
                    }else if(t.min > 30){                       //17:30:00 - 17:59:59
                    SECOND();
                    }                        
        }else if(t.hour >= 18 && t.hour <= 20){                       /////////////////////////////////////////////   6 PM - 8 PM            
                            
                    THIRD();

        }else if(t.hour >= 21 && t.hour <= 23){                       /////////////////////////////////////////////   9 PM - 11 PM            
                            
                    THIRD();                                       

        } 
        Serial.println(DayOfWeek);
        Serial.println(Day);
        Serial.println(Hour);
}
void MotionSensor(){/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        int PIR = analogRead(A1); 
        
        if(PIR >=500){
        
        SECOND(); 
        
        }else{ 
        
        FIRST();
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
