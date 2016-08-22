#include <SoftwareSerial.h>

//SoftwareSerial esp8266(4, 5); // RX, TX
HardwareSerial & esp8266 = Serial1;
void setup() {

    Serial.begin(9600);
    esp8266.begin(9600);

    while(!Serial); // Wait for the serial port to be ready.
    Serial.println("Serial ready!");
    
}

void loop() {

    while (esp8266.available() > 0) {
        Serial.write(esp8266.read());
    }

    while (Serial.available() > 0) {
        esp8266.write(Serial.read());
    }

}
