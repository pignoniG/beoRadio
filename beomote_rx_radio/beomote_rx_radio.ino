
#include <SPI.h>
#include <RH_RF95.h>
RH_RF95 rf95;

int lastCommand;

String recivedBeoMessage="00000000100000001";

#include "Beomotetxrx.h"

void setup() {
  
  Serial.begin(9600);
  
  if (!rf95.init())
  Serial.println("init failed");

  float frequency = 915.0; // Change the frequency here. 
  rf95.setFrequency(frequency);
    
}

void loop() {

  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

    // Should be a reply message for us now   
    if (rf95.recv(buf, &len))
   {

      recivedBeoMessage = (char*)buf;
      Serial.print("Recived_lora: ");
      Serial.println((char*)buf);
      if ( recivedBeoMessage == "0011101111000001010000000000000000000001"){Serial.println("1");}
      else if ( recivedBeoMessage == "0011101111000001010000000000000000000010"){Serial.println("2");}
      else if ( recivedBeoMessage == "0011101111000001010000000000000000000011"){Serial.println("3");}
      else if ( recivedBeoMessage == "0011101111000001010000000000000000000100"){Serial.println("4");}
      else if ( recivedBeoMessage == "0011101111000001010000000000000000000101"){Serial.println("5");}
      else if ( recivedBeoMessage == "0011101111000001010000000000000000000110"){Serial.println("6");}
      else if ( recivedBeoMessage == "0011101111000001010000000000000000000111"){Serial.println("7");}
      else if ( recivedBeoMessage == "0011101111000001010000000000000000001000"){Serial.println("8");}
      else if ( recivedBeoMessage == "0011101111000001010000000000000000001001"){Serial.println("9");}
      else if ( recivedBeoMessage == "0011101111000001010000000000000000001010"){Serial.println("10");}
     
   }
} 
