#include <Ethernet.h>
#include <EthernetUdp.h>
#include <SPI.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEE};

IPAddress ip(192,168,0,16);
unsigned int localPort = 9000;
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];
String datReg;
int packetSize;
EthernetUDP Udp;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Ethernet.begin(mac,ip);
  Udp.begin(localPort);
  delay(1500);
}

void loop() {
  // put your main code here, to run repeatedly:
  packetSize = Udp.parsePacket();
  
  if (packetSize > 0){
    Udp.read(packetBuffer,UDP_TX_PACKET_MAX_SIZE);
    String datReg(packetBuffer);
    if (datReg =="Blue"){
       Udp.beginPacket(Udp.remoteIP(),Udp.remotePort());
       Udp.print("You are Asking for Blue");
       Udp.endPacket();      
    }  
    if (datReg =="Green"){
       Udp.beginPacket(Udp.remoteIP(),Udp.remotePort());
       Udp.print("You are Asking for Green");
       Udp.endPacket();     
    }    
  }
 memset(packetBuffer, 0, UDP_TX_PACKET_MAX_SIZE);
}
