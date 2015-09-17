#include <SoftwareSerial.h>
SoftwareSerial XBee(2, 3); // RX, TX
int sensorPin = A0;

void setup() {
  // put your setup code here, to run once:
  XBee.begin(9600);
  Serial.begin(9600);
  pinMode(A0, INPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  float Temp;
  float sensorVal = analogRead(A0);
  float Rth = 9700*(5 - 5*sensorVal/1023)/(5*sensorVal/1023);
  float Temp1 = -.02*Rth + 250 -15;
  delay(1000);
  sensorVal = analogRead(A0);
  float Temp2 = -.02*Rth + 250 -15;
  delay(1000);
  sensorVal = analogRead(A0);
  float Temp3 = -.02*Rth + 250 -15;
  delay(1000);
  sensorVal = analogRead(A0);
  float Temp4 = -.02*Rth + 250 -15;
  delay(1000);
  sensorVal = analogRead(A0);
  float Temp5 = -.02*Rth + 250 -15;
  delay(1000);
  Temp = (Temp1 + Temp2 + Temp3 + Temp4 + Temp5)/5; 
  Serial.print("Temp: ");
  Serial.println(Temp);
  XBee.print("B Temp:");
  XBee.println(Temp);
}

