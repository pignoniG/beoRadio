
#include <SPI.h>
#include <RH_RF95.h>
RH_RF95 rf95;


bool verboseMode = false;

//TC9177 pins
int volLinkCK =16;    //clock 
int volLinkDATA =14;  //20bit settings for the volume ic
int volLinkSTB =15;   //"strobe" trigger read data in the shift register



int initialVolume = 17;
int maxVolume = 39;
int currentVolume = initialVolume;
int muteVolume = initialVolume;
bool mute = true;
bool isoff = true;
bool sendLora = false;
bool volumeChange = false;


//datalink pins
int dataPin = 3; //connect datalink to pin 3 directly, this is the "sense pin" 
int dataTalkPin = 6; //this is the "talk" pin 
                     //case (A) arduino is the slave. 
                     //if you are connecting to a beomaster, beocenter, beosound or similar this 
                     //pin will contoll a transistor that will pull the datalink to ground. 

                     //case (B) arduino is the master. if you are connected to a slave device, such as a master contorl link, 
                     //the arduino will provide the voltage for the datalink bus, and the slave device will pull it to gournd.
                     //for this use case connect pin 6 to datalink trough a 10k resistor
                      
                     //dont forghet to connect ground


bool arduinoIsMaster = true; // false for case (A), true for case (B)


bool LoraMessageRecived = false;
String recivedBeoMessage="";


#include "Beomotetxrx.h"


void setup() {

    if (verboseMode) {Serial.begin(250000);}

    //inizialise TC9177pins
    pinMode(volLinkCK, OUTPUT);
    pinMode(volLinkDATA, OUTPUT);
    pinMode(volLinkSTB, OUTPUT);

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
        sendLora = true;
        volumeChange = false;
        
        if (verboseMode) {
            Serial.print("Recived_datalink: ");
            Serial.println(beoMessage);
        }
        
        
        if ( beoMessage=="00000000101100100"){

            if(isoff==false) {
              if (verboseMode) { Serial.println("Volume DOWN");}

                if (mute){
                    mute = false;
                    currentVolume = muteVolume;
                }

                else if (currentVolume>0){
                    currentVolume=currentVolume-1;
                }
                
                volumeChange = true;
                
            }
            beoMessage= "";
            sendLora = false;
        }
        else if (beoMessage=="00000000101100000"){
            if(isoff==false) {
                if (verboseMode) {Serial.println("Volume UP");}

                if (mute){
                    mute = false;
                    currentVolume = muteVolume;
                }
           
                else if (currentVolume<maxVolume){
                    currentVolume=currentVolume+1;
                }
                
                volumeChange = true;
                
            }
            beoMessage= "";
            sendLora = false;
        }
        else if (beoMessage == "00000000100001101"){
            if (verboseMode) {Serial.println("MUTE");}
    
            if (mute){
                mute = false;
                currentVolume = muteVolume;
            }
            else{
                mute = true;
                muteVolume = currentVolume;
                currentVolume = 0;
            }
    
            if (isoff == true){
                isoff = false;
            }
            volumeChange = true;
        }
    
        else if (beoMessage == "00000000100001100"){
    
            if (verboseMode) {Serial.println("MUTE OFF");}
            mute = true;
            isoff = true;
            muteVolume = initialVolume;
            currentVolume = 0;
            volumeChange = true;  
        }
    
        else{
            if (isoff){
                isoff = false;
                mute = false;
                currentVolume =initialVolume;
            }
            if (mute){
                mute = false;
                currentVolume =  muteVolume;
            }   
            volumeChange = true; 
        }
        if (volumeChange){

            if (verboseMode) {
              Serial.print("Volume = ");
              Serial.println(currentVolume);
            }
            
            volumeCalc(currentVolume);
        }
        if (sendLora){
            uint8_t data[50];  beoMessage.toCharArray(data,50); // or 
            rf95.send(data, sizeof(data));
            rf95.waitPacketSent();
            if (verboseMode) {Serial.println("Sent_Lora");}
         }
    

        reset();
    }
    

    if (rf95.recv(buf, &len)){
        LoraMessageRecived = true;
        recivedBeoMessage = (char*)buf;
        if (verboseMode) {
           Serial.print("Rec_lora: ");
           Serial.println((char*)buf);
        }
       
    }

    if (LoraMessageRecived ){
        noInterrupts();
        
        if (recivedBeoMessage=="0011101111000001010000000000001011111111"){
            if (verboseMode) { Serial.println("MUTE OFF (beomaster)");}
            mute = true;
            isoff = true;
            muteVolume = initialVolume;
            currentVolume = 0;
            volumeCalc(currentVolume);
        }
        beoTalk(recivedBeoMessage,arduinoIsMaster);
        LoraMessageRecived = false;
        //delay(10);
        reset();
        interrupts();
   }
} 




void volumeCalc(int volume) {
    int volumefine= -1;
    int volumeCoarse = 0;

    for (int i = 0; i <= volume; i++) {
        volumefine=volumefine+1;
        if (volumefine>4) {
            volumefine= 0;
            volumeCoarse=volumeCoarse+1;
            if (volumeCoarse >7){
                volumeCoarse = 0;
            }
        }
    }
    //Serial.println(volumeCoarse);
    //Serial.println(volumefine);
    volumeCommand(volumefine,volumeCoarse);

}




void volumeCommand(int volumeFine, int volumeCoarse){
  
    char *VolumeFineC[] = {"00001","00010","00100","01000","10000"};
    char *VolumeCoarseC[] = {"00000001","00000010","00000100","00001000","00010000","00100000","01000000","10000000"}; 
  
    //Serial.println("volumeFine");
    //Serial.println(VolumeFineC[volumeFine]);
  
    //Serial.println("volumeCoarse");
    //Serial.println(VolumeCoarseC[volumeCoarse]);
  
    char volMessageStart[] ="110";
    for (int i=0;i<3;i++) {
        sendVolumeBit(volMessageStart[i]);
    }
    for (int i=0;i<5;i++) {
        sendVolumeBit(VolumeFineC[volumeFine][i]);
    }
    for (int i=0;i<8;i++) {
        sendVolumeBit(VolumeCoarseC[volumeCoarse][i]);
    }
    char volMessageEnd[] ="0001";
    for (int i=0;i<4;i++) {
        sendVolumeBit(volMessageEnd[i]);
    }
    volumeSet();
}

void sendVolumeBit( char VolBit){
    if (VolBit == '0') {
        digitalWrite(volLinkDATA,0);
    }
    if (VolBit == '1') {
        digitalWrite(volLinkDATA,1);}
    _delay_us(20);
    digitalWrite(volLinkCK, 1);
    _delay_us(10);
    digitalWrite(volLinkCK, 0);
    _delay_us(10);
}


void volumeSet(){
    digitalWrite(volLinkDATA, 0);
    _delay_us(10);
    digitalWrite(volLinkSTB, 1);
    _delay_us(10);
    digitalWrite(volLinkSTB, 0);
    _delay_us(10);
}
