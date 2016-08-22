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

#define BUFFER_SIZE 128
char buffer[BUFFER_SIZE];

void setup() { 
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
  connectWiFi("minyoung", "01199");
  
  //test if the module is ready
  dbgTerminal.print("AT : ");
  dbgTerminal.println( GetResponse("AT",100) );
  
  //Change to mode 1
  dbgTerminal.print("AT+CWMODE=1 : ");
  dbgTerminal.println( GetResponse("AT+CWMODE=1",100) );
        
  //set the multiple connection mode
  dbgTerminal.print(F("AT+CIPMUX=1 : "));
  dbgTerminal.println( GetResponse("AT+CIPMUX=1",100) );
  
  //set the server of port 80 check "no change" or "OK"
  dbgTerminal.print(F("AT+CIPSERVER=1,8888 : "));
  dbgTerminal.println( GetResponse("AT+CIPSERVER=1,8888", 100) );
 
  //set time out
  dbgTerminal.print("AT+CIPSTO=15 : ");
  dbgTerminal.println( GetResponse("AT+CIPSTO=15",300) );
  
   //print the ip addr
  dbgTerminal.print(F("ip address : "));
  dbgTerminal.println( GetResponse("AT+CIFSR", 1000) );
  delay(200);

   
  dbgTerminal.println();
  dbgTerminal.println(F("Start Webserver"));

  digitalWrite(ledPin,ledState);  
}

void loop() {
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
      dbgTerminal.print("pb?: ");
      dbgTerminal.println(pb);
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
        
        homepage(ch_id);

      } else if (strncmp(pb, "GET / ", 6) == 0) {
        dbgTerminal.print(millis());
        dbgTerminal.print(" : ");
        dbgTerminal.println(buffer);
        dbgTerminal.print( "get Status from ch:" );
        dbgTerminal.println(ch_id);
        
        delay(100);
        clearSerialBuffer();

        homepage(ch_id);
      }
    }
  }
  clearBuffer();
}

void homepage(int ch_id) {
  String Header;

  Header =  "HTTP/1.1 200 OK\r\n";
  Header += "Content-Type: text/html\r\n";
  Header += "Connection: close\r\n";  
  Header += "Refresh: 5\r\n";
  
  String Content;
  
  Content = "<body bgcolor=\"#99ff99\" alink=\"#EE0000\" link=\"#0000EE\" text=\"#000000\"vlink=\"#551A8B\">";
  Content += "<title> ESP8266 test </title>";
  Content += "<H1>";
  Content += "ESP8266 Web Server Test Page, Reloads = ";
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
  delay(700);
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
