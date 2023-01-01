
#include <SPI.h>
#include <RH_RF95.h>
RH_RF95 rf95;

bool verboseMode = false;

int dataPin = 3; //connect datalink to pin 3 directly, this is the "sense pin" 

int dataTalkPin = 6; //this is the "talk" pin 
                     //case (A) arduino is the slave. 
                     //if you are connecting to a beomaster, beocenter, beosound or similar this 
                     //pin will contoll a transistor that will pull the datalink to ground. 

                     //case (B) arduino is the master. if you are connected to a slave device, such as a master contorl link, 
                     //the arduino will provide the voltage for the datalink bus, and the slave device will pull it to gournd.
                     //for this use case connect pin 6 to datalink trough a 10k resistor
                      
                     //dont forghet to connect ground


bool arduinoIsMaster = false; // false for case (A), true for case (B)


bool LoraMessageRecived = false;
String recivedBeoMessage="";


#include "Beomotetxrx.h"


void setup() {

  if (verboseMode) {Serial.begin(250000);}
  initialize(dataPin);//inizialise the listen pin
  
  initializeTalking(dataTalkPin,arduinoIsMaster); //inizialise the talking pin

  if (!rf95.init())
    if (verboseMode) {Serial.println("init failed");}

  float frequency = 915.0; // Change the frequency here. 
  rf95.setFrequency(frequency);
    rf95.setTxPower(2);
}

void loop() {

  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  if (dataLinkReceived) {
      if (verboseMode) {Serial.print("Recived_datalink: ");
      Serial.println(beoMessage);}

    uint8_t data[50];  beoMessage.toCharArray(data,50); // or 

    rf95.send(data, sizeof(data));
    rf95.waitPacketSent();
   
    if (verboseMode) {Serial.println("Sent_Lora");}
    reset();
    }
    
    // Should be a reply message for us now   
    if (rf95.recv(buf, &len))
   {
      LoraMessageRecived = true;
      recivedBeoMessage = (char*)buf;
        if (verboseMode) {
          Serial.print("Rec_lora: ");
          Serial.println((char*)buf);}
   }

  if (LoraMessageRecived ){
        noInterrupts();
        //Serial.println(recivedBeoMessage.length());
        beoTalk(recivedBeoMessage,arduinoIsMaster);
        LoraMessageRecived = false;
        delay(10);
        reset();
        interrupts();
   }
} 
