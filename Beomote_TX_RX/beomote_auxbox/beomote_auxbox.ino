#include "Beomote.h"
int irPin = 10; //connect powerlink to pin 10 and ground to ground
int lastCommand;
unsigned long lastTime;
void setup() {
  Serial.begin(9600);

   pinMode(9, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);

  digitalWrite(9, HIGH);
  digitalWrite(8, LOW);
  digitalWrite(11, LOW);
  digitalWrite(12, LOW);
  
  Beo.initialize(irPin);
}
void loop() {
  BeoCommand cmd;
  if (Beo.receive(cmd)) {
    //uncomment all below to see an entire command, you will have to look for patterns to understnad what means what,




  
    if (cmd.addressTo == 0x5E && cmd.addressFrom == 0x0)
  {
  Serial.println("A1");
  digitalWrite(9, HIGH);
  digitalWrite(8, LOW);
  digitalWrite(11, LOW);
  digitalWrite(12, LOW);
  }

if (cmd.addressTo == 0xDE && cmd.addressFrom == 0x45)
  {
  Serial.println("A2");
  digitalWrite(9, LOW);
  digitalWrite(8, HIGH);
  digitalWrite(11, LOW);
  digitalWrite(12, LOW);
  }
if (cmd.addressTo == 0x5E && cmd.addressFrom == 0xAA)
  {
  Serial.println("A3");
   digitalWrite(9, LOW);
  digitalWrite(8, LOW);
  digitalWrite(11, HIGH);
  digitalWrite(12, LOW);
  }
if (cmd.addressTo == 0x5E && cmd.addressFrom == 0x66)
  {
  Serial.println("A4");
   digitalWrite(9, LOW);
  digitalWrite(8, LOW);
  digitalWrite(11, LOW);
  digitalWrite(12, HIGH);
  }

    if (cmd.addressTo == 0x9E && cmd.addressFrom == 0x8)
  {
  Serial.println("A5");
  }

  
  



  }
}
