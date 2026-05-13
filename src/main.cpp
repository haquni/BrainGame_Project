// C++ code
//
#include <Arduino.h>
int buttonTeam1 = 0;
int buttonTeam2 = 0;
int buttonTeam3 = 0;
int buttonTeam4 = 0;
bool led1 = false;
bool led2 = false;
bool led3 = false;
bool led4 = false;
int LastState = 0;
int StartStop = 0;
int tmr = 0;
bool LastStartStop = false;
bool LastStartStop1 = false;
const uint8_t Ledout[] = {2,3,4,5};
const uint8_t Buttinp[] = {9,10,11,12,13};
void setup()
{
  for (uint8_t p : Ledout) pinMode(p, OUTPUT);
  for (uint8_t p : Buttinp) pinMode(p, INPUT);
  pinMode(7,OUTPUT);
}

void loop()
{
buttonTeam1 = digitalRead(Buttinp[0]);
buttonTeam2 = digitalRead(Buttinp[1]);
buttonTeam3 = digitalRead(Buttinp[2]);
buttonTeam4 = digitalRead(Buttinp[3]);
StartStop = digitalRead(8);





if (StartStop == HIGH && LastState == LOW)
{
  LastStartStop = !LastStartStop; 
  delay(200);  
}
LastState = StartStop;






if (LastStartStop)
{
  if(LastStartStop1 == false){
    tmr = millis();
  }


  if ((buttonTeam1 == HIGH  led1 == true) && !led2 && !led3 && !led4){
  digitalWrite(2,HIGH);
  led1 = true;
  }
  
  if ((buttonTeam2 == HIGH  led2 == true) && !led1 && !led3 && !led4){
  digitalWrite(3,HIGH);
  led2 = true;
  }
  
  if ((buttonTeam3 == HIGH  led3 == true) && !led1 && !led2 && !led4){
  digitalWrite(4,HIGH);
  led3 = true;
  }
  
  if ((buttonTeam4 == HIGH  led4 == true) && !led1 && !led2 && !led3){
  digitalWrite(5,HIGH);
  led4 = true;
  }

  LastStartStop1 == true;
  if(millis()-tmr >= 3000){
    digitalWrite(7,HIGH);
  }

    
}else{
digitalWrite(2,LOW);
digitalWrite(3,LOW);
digitalWrite(4,LOW);
digitalWrite(5,LOW);
LastStartStop1 == false;
led1 = false;
led2 = false;
led3 = false;
led4 = false;

if (buttonTeam1 == HIGH){
  digitalWrite(2,HIGH);
  delay(100);
  digitalWrite(2,LOW);
  delay(100);
  digitalWrite(2,HIGH);
  
  }
  
  if (buttonTeam2 == HIGH){
  digitalWrite(3,HIGH);
  delay(100);
  digitalWrite(3,LOW);
  delay(100);
  digitalWrite(3,HIGH);
  
  }
  
  if (buttonTeam3 == HIGH){
  digitalWrite(4,HIGH);
  delay(100);
  digitalWrite(4,LOW);
  delay(100);
  digitalWrite(4,HIGH);
  
  }
  
  if (buttonTeam4 == HIGH){
  digitalWrite(5,HIGH);
  delay(100);
  digitalWrite(5,LOW);
  delay(100);
  digitalWrite(5,HIGH);
  
  }

 
















}
  
  
  
  

  
  
  
  
  
  
}