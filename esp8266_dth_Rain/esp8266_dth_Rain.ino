#include "U8glib.h"
#include <SimpleDHT.h>

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_DEV_0|U8G_I2C_OPT_NO_ACK|U8G_I2C_OPT_FAST);  // Fast I2C / TWI 

// lowest and highest sensor readings:
const int sensorMin = 0;     // sensor minimum
const int sensorMax = 1024;  // sensor maximum


// for DHT11, 
//      VCC: 5V or 3V
//      GND: GND
//      DATA: 2
int pinDHT11 = 7;
SimpleDHT11 dht11;

//Leonardo
//Serial_ & dbgTerminal = Serial;
//HardwareSerial & espSerial = Serial1;

////UNO & M328P
//#include <SoftwareSerial.h>
//SoftwareSerial dbgTerminal(3, 4); // RX, TX
//HardwareSerial & espSerial = Serial;

//
////MEGA2560 
HardwareSerial & dbgTerminal = Serial;
HardwareSerial & espSerial = Serial1;

// set pin numbers:
const int ledPin =  13;      // the number of the LED pin
const int ESP8266_CHPD = 8;

// Variables will change:
int ledState = HIGH;             // ledState used to set the LED



int int_temperature;
int int_humidity;
  
#define BUFFER_SIZE 128
char buffer[BUFFER_SIZE];

////////////////////OLEDDRAW///////////
void draw(int tempf, int tempc,int rain) {
  
  // graphic commands to redraw the complete screen should be placed here  
  u8g.setFont(u8g_font_unifont);
  //u8g.setFont(u8g_font_osb21);
  
  //u8g.drawStr( 0, 22, "Hello World");
  u8g.setFont(u8g_font_unifont);
  u8g.drawStr( 0, 14, "Temperature ");//Temperature
  u8g.setPrintPos(0, 32);  //110,22 for eol
  u8g.print(tempf);
  u8g.drawStr( 25, 32, "C");
  u8g.setPrintPos(0, 46);
  u8g.print(tempc);
  u8g.drawStr( 25, 46, "H");
  u8g.drawStr( 0, 60, "Rain");
  if (rain == 0){
   u8g.drawStr( 35, 60, "Not Raining"); 
  }
  else if(rain ==1){
    u8g.drawStr(35, 60, "Rain Warning");    
  }else u8g.drawStr( 35, 60, "Flood");
  
}

void setup() { 
  // assign default color value
  if ( u8g.getMode() == U8G_MODE_R3G3B2 ) {
    u8g.setColorIndex(255);     // white
  }
  else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) {
    u8g.setColorIndex(3);         // max intensity
  }
  else if ( u8g.getMode() == U8G_MODE_BW ) {
    u8g.setColorIndex(1);         // pixel on
  }
  else if ( u8g.getMode() == U8G_MODE_HICOLOR ) {
    u8g.setHiColorByRGB(255,255,255);
  }
  
  pinMode(ledPin, OUTPUT);  
  pinMode(ESP8266_CHPD, OUTPUT);
      
  dbgTerminal.begin(9600); // Serial monitor
  espSerial.begin(9600); // ESP8266
    
  //while (!dbgTerminal) {
   // ; // wait for serial port to connect. Needed for Leonardo only
  //}

  dbgTerminal.println(F("ESP8266 demo."));

  //hardReset();
  //delay(2000);
  
  clearSerialBuffer();
  
  //connect to router
 // connectWiFi("minyoung", "01199");
  
  /*test if the module is ready
  dbgTerminal.print("AT : ");
  dbgTerminal.println( GetResponse("AT",10) );
  
  //Change to mode 1
  dbgTerminal.print("AT+CWMODE=1 : ");
  dbgTerminal.println( GetResponse("AT+CWMODE=1",10) );
  */      
  //set the multiple connection mode
  dbgTerminal.print(F("AT+CIPMUX=1 : "));
  dbgTerminal.println( GetResponse("AT+CIPMUX=1",10) );
  
  //set the server of port 80 check "no change" or "OK"
  dbgTerminal.print(F("AT+CIPSERVER=1,8888 : "));
  dbgTerminal.println( GetResponse("AT+CIPSERVER=1,8888", 20) );
 
  //set time out
  dbgTerminal.print("AT+CIPSTO=15 : ");
  dbgTerminal.println( GetResponse("AT+CIPSTO=15",100) );
  
   //print the ip addr
  dbgTerminal.print(F("ip address : "));
  dbgTerminal.println( GetResponse("AT+CIFSR", 300) );
 
  delay(200);
  
   
  dbgTerminal.println();
  dbgTerminal.println(F("Start Webserver"));

  digitalWrite(ledPin,ledState);  
}

void loop() {
  // read the sensor on analog A0:
  int sensorReading = analogRead(A0);
  // map the sensor range (four options):
  // ex: 'long int map(long int, long int, long int, long int, long int)'
  int range = map(sensorReading, sensorMin, sensorMax, 0, 3);

  // start working...
  //Serial.println("=================================");
  //Serial.println("Sample DHT11...");
  
  // read without samples.
  byte temperature = 0;
  byte humidity = 0;
  if (dht11.read(pinDHT11, &temperature, &humidity, NULL)) {
    //Serial.print("Read DHT11 failed.");
    return;
  }
  //Serial.print("Sample OK: ");
  //Serial.print((int)temperature); Serial.print(" *C, "); 
  int_temperature=(int)temperature;
  int_humidity=(int)humidity;
  
  //Serial.print((int)humidity); Serial.println(" %");
  
  // DHT11 sampling rate is 1HZ.
  
  // picture loop
  u8g.firstPage();  
  do {
    draw(temperature,humidity,range);
  } while( u8g.nextPage() );
  
  delay(500);

  int ch_id, packet_len;
  char *pb;  
  espSerial.readBytesUntil('\n', buffer, BUFFER_SIZE);
  
  if(strncmp(buffer, "+IPD,", 5)==0) {
    // request: +IPD,ch,len:data
    sscanf(buffer+5, "%d,%d", &ch_id, &packet_len);
    if (packet_len > 0) {
      // read serial until packet_len character received
      // start from :
      pb = buffer+5;
      while(*pb!=':') pb++;
      pb++;
      
      if (strncmp(pb, "GET /led", 8) == 0) {
        dbgTerminal.print(millis());
        dbgTerminal.print(" : ");
        dbgTerminal.println(buffer);
        dbgTerminal.print( "get led from ch :" );
        dbgTerminal.println(ch_id);
 
        delay(100);
        clearSerialBuffer();
        
       if (ledState == LOW)
          ledState = HIGH;
       else
          ledState = LOW;
        digitalWrite(ledPin, ledState);
        
        homepage(ch_id,int_temperature,int_humidity,range);

      } else if (strncmp(pb, "GET / ", 6) == 0) {
        dbgTerminal.print(millis());
        dbgTerminal.print(" : ");
        dbgTerminal.println(buffer);
        dbgTerminal.print( "get Status from ch:" );
        dbgTerminal.println(ch_id);
        
        delay(100);
        clearSerialBuffer();

        homepage(ch_id,int_temperature,int_humidity,range);
      }
    }
  }
  clearBuffer();
}

void homepage(int ch_id,int tempc, int tempf, int range) {
  String raing;
  String Header;
  if(range ==0){
     raing="Not Rainging";
  }
  else if(range ==1){
    raing = "Rain Warning";
  }else raing ="Flood";

  Header =  "HTTP/1.1 200 OK\r\n";
  Header += "Content-Type: text/html\r\n";
  Header += "Connection: close\r\n";  
  Header += "Refresh: 5\r\n";
  
  String Content;
  
  Content = "<body bgcolor=\"#99ff99\" alink=\"#EE0000\" link=\"#0000EE\" text=\"#000000\"vlink=\"#551A8B\">";
  Content += "<title> ESP8266 test </title>";
  Content += "<H1>";
  Content += "ESP8266 Web Server Test Page </H1> ";
  Content +="<p>";
  Content += "temperature: ";
  Content +=String(tempc)+"C";
  Content +="</br>";
  Content += "humidity: ";
  Content +=String(tempf)+"H";
  Content +="<br></P>"; 
  Content += "Rain: ";
  Content += raing;
  Content +="</br>";
  Content +="<p> D";
  Content += String(ledState);
  Content +="</p>";
  Header += "Content-Length: ";
  Header += (int)(Content.length());
  Header += "\r\n\r\n";
  
  
  espSerial.print("AT+CIPSEND=");
  espSerial.print(ch_id);
  espSerial.print(",");
  espSerial.println(Header.length()+Content.length());
  delay(10);

  
  // for debug buffer serial error
  //while (espSerial.available() >0 )  {
  //  char c = espSerial.read();
  //  dbgTerminal.write(c);
  //  if (c == '>') {
  //      espSerial.print(Header);
  //      espSerial.print(Content);
  //  }
  //}
  
  if (espSerial.find(">")) {
      espSerial.print(Header);
      espSerial.print(Content);
      delay(10);
   }
 
//  Serial1.print("AT+CIPCLOSE=");
//  Serial1.println(ch_id);


}



// Get the data from the WiFi module and send it to the debug serial port
String GetResponse(String AT_Command, int wait){
  String tmpData;
  
  espSerial.println(AT_Command);
  //delay(wait);
  delay(1000);
  while (espSerial.available() >0 )  {
    char c = espSerial.read();
    tmpData += c;
    
    if ( tmpData.indexOf(AT_Command) > -1 )         
      tmpData = "";
    else
      tmpData.trim();       
          
   }
   return tmpData;
}

boolean hardReset() {
  String tmpData;
  
  digitalWrite(ESP8266_CHPD,LOW);
  delay(100);
  digitalWrite(ESP8266_CHPD,HIGH);
  delay(1000);
    
  while ( espSerial.available() > 0 ) {
    char c = espSerial.read();
    tmpData +=c;
    espSerial.write(c);
    if ( tmpData.indexOf("Ready") > -1 ) {
      //Serial.println("Ready");
        clearBuffer();
        return 1;
    } 
  }
}

void clearSerialBuffer(void) {
       while ( espSerial.available() > 0 ) {
         espSerial.read();
       }
}

void clearBuffer(void) {
       for (int i =0;i<BUFFER_SIZE;i++ ) {
         buffer[i]=0;
       }
}
         
boolean connectWiFi(String NetworkSSID,String NetworkPASS) {
  String cmd = "AT+CWJAP=\"";
  cmd += NetworkSSID;
  cmd += "\",\"";
  cmd += NetworkPASS;
  cmd += "\"";
  
  dbgTerminal.println(cmd); 
  dbgTerminal.println(GetResponse(cmd,10));
}
