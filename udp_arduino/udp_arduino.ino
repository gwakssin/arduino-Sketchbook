
#include <SPI.h>        

#include <Ethernet.h>

#include <EthernetUdp.h>



// Enter a MAC address and IP address for your controller below.

// The IP address will be dependent on your local network:

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

IPAddress ip(192, 168, 0, 16);



unsigned int localPort = 9999;      // local port to listen on



// An EthernetUDP instance to let us send and receive packets over UDP

EthernetUDP Udp;
char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; //buffer to hold incoming packet,



void setup() {

  // start the Ethernet and UDP:
  Serial.begin(9600);
  Ethernet.begin(mac,ip);

  Udp.begin(localPort);

}



void loop() {
    IPAddress remote = Udp.remoteIP();
    int packetSize = Udp.parsePacket();
    if (packetSize)
    {
     Udp.read(packetBuffer,UDP_TX_PACKET_MAX_SIZE);

      Serial.print("REcevied packet of size");
      Serial.println(packetSize);
      Serial.println (packetBuffer);
    }
    Udp.beginPacket("192.168.0.5", 5000);

    Udp.write("hello");

    Udp.endPacket();
    
    delay(50);

}
