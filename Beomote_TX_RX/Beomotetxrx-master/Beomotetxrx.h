#include <Arduino.h>
#include <avr/interrupt.h>
#include <avr/io.h>

// Common divisors for 3125 = 1, 5, 25, 125, 625, 3125
#define TICK 625

// Defining the Bang & Olufsen commands
#define BEO_ZERO    (3125 / TICK)
#define BEO_SAME    (6250 / TICK)
#define BEO_ONE     (9375 / TICK)
#define BEO_STOP    (12500 / TICK)
#define BEO_START   (15625 / TICK)


int dataLinkPin; //the pin connected to read the datalink bus
int talkPin;     //te pin cponnected to write to the datalink bus

int lastState ;
int lastBeoBit;

boolean dataLinkReceived;
boolean talking = false;

String beoMessage;

int timer;
int index;

void reset() {
  index = -1;

  dataLinkReceived = 0;
  beoMessage ="";
  lastState = HIGH;
}

        
void initialize(int dPin) {
  dataLinkPin = dPin;
  // Setting the pinmode of the datalink pin and resetting the listener
  pinMode(dataLinkPin, INPUT);

  reset();

  // Clearing Control Register A
  TCCR1A = 0;
  // Setting the phase and frequency correct pwm, and stopping the timer
  TCCR1B = _BV(WGM13);

  long cycles = (F_CPU / 2000000) * TICK;
  ICR1 = cycles;

  TCCR1B &= ~(_BV(CS10) | _BV(CS11) | _BV(CS12));

  // Setting the timer overflow interrupt enable bit
  TIMSK1 = _BV(TOIE1);

  // Resetting clock select register, and starts the clock with no prescale
  TCCR1B |= _BV(CS10);
}




void timerISR() {

    
  if (dataLinkReceived)
    return;

  timer++;

  int dlData = digitalRead(dataLinkPin);
 

  if (dlData == LOW && lastState == HIGH) {


    int beoCode = 0;
    int beoBit;

    if (abs(timer - BEO_ZERO) < 2) {
      beoCode = BEO_ZERO;
      beoBit = lastBeoBit = 0;
    }
    else if (abs(timer - BEO_SAME) < 2) {
      beoCode = BEO_SAME;
      beoBit = lastBeoBit;
    }
    else if (abs(timer - BEO_ONE) < 2) {
      beoCode = BEO_ONE;
      beoBit = lastBeoBit = 1; 
    }
    else if (abs(timer - BEO_STOP) < 2) {
      beoCode = BEO_STOP;
    }
    else if (abs(timer - BEO_START) < 2) {
      beoCode = BEO_START;

    }
    else
      index = -1;


    if (index == -1) {
      if (beoCode == BEO_START)
        index = 0;
    }

    //most messages are 40 bits, but it dependes so we are dinamically stopping when we get to a stop bit 

    else if (index < 100) {
       if (beoCode == BEO_STOP){
        
        dataLinkReceived = 1;}
       else{
        beoMessage +=beoBit;
        index++;
        }

    }
    else if (index > 100) {
        //Giving up, cannot be more that 100 bits
        reset();
    }


  	timer = 0;
	lastState = LOW;
}
 else if (dlData == HIGH && lastState == LOW) {
    lastState = HIGH;
  }
}

ISR(TIMER1_OVF_vect) {
  timerISR();
}

void initializeTalking(int tpin, bool isMaster) {
  talkPin = tpin;
  
  pinMode(talkPin, OUTPUT);
  if (isMaster)
  {
    digitalWrite(talkPin, HIGH);
  }
  else{ digitalWrite(talkPin, LOW);

  }
  
}

void beoTalk( String recivedBeoMessage, bool isMaster){

	
	if (recivedBeoMessage=="0011101111000001010010001001100000101100" && isMaster)
	{
		recivedBeoMessage="0011101111000001010000000000000000000001";
		if (verboseMode) {Serial.println("radio on fix");}
	}

     int l = 1;
     int h = 0;

     if (isMaster)
     {
         l = 0;
         h = 1;
     }

     delay(20);
     noInterrupts();
     float correction(00);
     talking = true;
     char prev = '1';
     char curr;

     // sending a start bit
     digitalWrite(talkPin, l);
     _delay_us(1542);
     digitalWrite(talkPin, h);
     _delay_us(1542);
     digitalWrite(talkPin, l);
     _delay_us(1542);
     digitalWrite(talkPin, h);
     _delay_us(1542);
     digitalWrite(talkPin, l);
     _delay_us(1542);
     digitalWrite(talkPin, h);
     _delay_us(14025);
     digitalWrite(talkPin, l);
     _delay_us(1542);
     // end of start bit


    //sending the message
     for (int i = 0; i < recivedBeoMessage.length(); i++) {

        digitalWrite(talkPin, h);

        curr = recivedBeoMessage.charAt(i);

            if (curr == prev)
                {_delay_us(4708);}
            else{
                 if (curr == '1')
                    { _delay_us(7833);}
                 else
                    {_delay_us(1583);}
            }

        digitalWrite(talkPin, l);
        _delay_us(1542);
        prev = curr;
    }

    // sending a stop bit

    digitalWrite(talkPin, h);
    _delay_us(11500);
    digitalWrite(talkPin, l);
    _delay_us(1542);
    digitalWrite(talkPin, h);
    // end of stop bit

    if (verboseMode) {
      Serial.print("Sent_datalink: ");
      Serial.println(recivedBeoMessage);}
    talking = false;
    interrupts();


}

