#include <Servo.h>

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

Servo myservo;
char b[4];

//Debug
int serial_putc( char c, FILE * ) 
{
  Serial.write( c );
  return c;
} 

//Debug
void printf_begin(void)
{
  fdevopen( &serial_putc, 0 );
}

//nRF24 Cableado utilizado. El pin 9 es CE y 10 a CSN/SS
//     CE       -> 9
//     SS       -> 10
//     MOSI     -> 11
//     MISO     -> 12
//     SCK      -> 13

RF24 radio(9,10);

const uint64_t pipes[2] = {0x65646f4e32LL,0x65646f4e31LL};



void setup(void) {
  myservo.attach(6);
  myservo.write(5);
  Serial.begin(9600);
  printf_begin();      //Debug

  //nRF24 configuración
  radio.begin();
  radio.setChannel(0x76);
  radio.setAutoAck(1);
  radio.setRetries(15,15);
  radio.setPayloadSize(32);
  radio.enableDynamicPayloads();
  radio.openReadingPipe(1,pipes[0]);
  radio.openWritingPipe(pipes[1]);
  radio.startListening();
  radio.printDetails(); //Debug
  radio.powerUp();
}

void loop() {
      char rev[32] = {0};
      Serial.println("String loop.Radio.on."); 
      char send_message[]="ready"; 
      radio.stopListening();
      radio.write(&send_message,sizeof(send_message));
      delay(15);
      radio.startListening();           
      while (radio.available()) {      
           Serial.println("redy");                   
           //radio.read(rev, sizeof(rev) );
           radio.read(rev, radio.getDynamicPayloadSize());  
           String mystring = String(rev);
           Serial.println(mystring);
           if (mystring == "ON"){
              Serial.println("start");
              myservo.write(62);
              delay(30000); 
              myservo.write(0);
              delay(15);
              radio.stopListening();
              char send_message[]="success";
              radio.write(&send_message,sizeof(send_message));
              radio.startListening(); 
              Serial.println(send_message);
              Serial.println("end"); 
           }
    Serial.println("READY");
      }
    Serial.println("Rev_ERROR");
  delay(1000); 
}

