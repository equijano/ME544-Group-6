#include <SoftwareSerial.h>
SoftwareSerial XBee(2, 3); // RX, TX
int sensorPin = 13;
void setup() {
  // put your setup code here, to run once:
  XBee.begin(9600);
  Serial.begin(9600);
  pinMode(13, OUTPUT);
  digitalWrite(sensorPin, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  //int input = XBee.read();
  if (XBee.available() > 0) {
    //command = XBee.read();
    //int input = XBee.read();
    int s = XBee.read();
    XBee.write(s);
    int input = s;
    //Serial.println(input);
    //XBee.println(input);
  //input = 1;
  if(input == '0'){
    digitalWrite(sensorPin, LOW);
    }else{
      digitalWrite(sensorPin, HIGH);
      }
   delay(1000);   
}
delay(100);
}
