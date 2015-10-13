#include <SoftwareSerial.h>
SoftwareSerial XBee(2, 3); // RX, TX
int sensorPin = 13;
void setup() {
  // put your setup code here, to run once:
  XBee.begin(9600);
  Serial.begin(9600);
  pinMode(13, OUTPUT);
  digitalWrite(sensorPin, LOW);
  XBee.write("PLease only use 1 or 0 to control");
}

void loop() {
  // put your main code here, to run repeatedly:
  //int input = XBee.read();
  //digitalWrite(sensorPin, LOW);
  while (XBee.available() > 0) {
    //command = XBee.read();
    //int input = XBee.read();
   // digitalWrite(sensorPin, LOW);
    int s = XBee.read();
    XBee.write(s);
    int input = s;
    //Serial.println(input);
    //XBee.println(input);
    //input = 1;
    if(input == '0'){
      digitalWrite(sensorPin, LOW);
      delay(1000);
      XBee.write("LED is on");
      }
        if(input == '1') {
        digitalWrite(sensorPin, HIGH);
        XBee.write("LED is off");
        }
       
      
  }

   //digitalWrite(sensorPin,comm);
   delay(1000);
     
}

