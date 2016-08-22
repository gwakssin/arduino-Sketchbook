#include "U8glib.h"
#include <SimpleDHT.h>


U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_DEV_0|U8G_I2C_OPT_NO_ACK|U8G_I2C_OPT_FAST);	// Fast I2C / TWI 

// lowest and highest sensor readings:
const int sensorMin = 0;     // sensor minimum
const int sensorMax = 1024;  // sensor maximum


// for DHT11, 
//      VCC: 5V or 3V
//      GND: GND
//      DATA: 2
int pinDHT11 = 2;
SimpleDHT11 dht11;

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
  Serial.begin(9600);
  
  
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
  
}

void loop() {
  // read the sensor on analog A0:
  int sensorReading = analogRead(A0);
  // map the sensor range (four options):
  // ex: 'long int map(long int, long int, long int, long int, long int)'
  int range = map(sensorReading, sensorMin, sensorMax, 0, 3);
  
  // range value:
  switch (range) {
 case 0:    // Sensor getting wet
    Serial.println("Flood");
    break;
 case 1:    // Sensor getting wet
    Serial.println("Rain Warning");
    
    break;
 case 2:    // Sensor dry - To shut this up delete the " Serial.println("Not Raining"); " below.
    Serial.println("Not Raining");
    break;
  }
  
  // start working...
  Serial.println("=================================");
  Serial.println("Sample DHT11...");
  
  // read without samples.
  byte temperature = 0;
  byte humidity = 0;
  if (dht11.read(pinDHT11, &temperature, &humidity, NULL)) {
    Serial.print("Read DHT11 failed.");
    return;
  }
  
  Serial.print("Sample OK: ");
  Serial.print((int)temperature); Serial.print(" *C, "); 
  Serial.print((int)humidity); Serial.println(" %");
  
  // DHT11 sampling rate is 1HZ.
  
  // picture loop
  u8g.firstPage();  
  do {
    draw(temperature,humidity,range);
  } while( u8g.nextPage() );
  
  
  
  delay(500);
}
