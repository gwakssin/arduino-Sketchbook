int LED = 13;
boolean LEDst = false;

//always high
int CH_PD_8266 = 6;

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LEDst);
  pinMode(CH_PD_8266, OUTPUT);
  digitalWrite(CH_PD_8266, HIGH);
}

void loop() {
  while (Serial.available() > 0) {
    char a = Serial.read();
    Serial1.write(a);
  }
  
}

void serialEvent1() {
  while (Serial1.available() > 0) {
    char a = Serial1.read();
    Serial.write(a);
    ToggleLED();
  }
}

void ToggleLED(){
  digitalWrite(LED, LEDst = !LEDst);
}
