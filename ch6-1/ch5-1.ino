#include <SoftwareSerial.h>
SoftwareSerial XBee(2, 3); // RX, TX

int id = 3;
boolean isLeader;
int leaderId;

void setup() {
  // put your setup code here, to run once:
  XBee.begin(9600);
  Serial.begin(9600);
  leaderId = id;
}

void loop() {
  if(id == leaderId){
    isLeader = true;
  }
  if(isLeader == true){
     XBee.write(id); 
  }
  if (XBee.available() > 0) {
    int input = XBee.read();
    Serial.print("aaaaaaaaaaaaaaaaaaaaaaaaa");
    Serial.println(input);
    if(input > leaderId){
      isLeader = false;
      leaderId = input;  
    } else {
      isLeader = true;
      leaderId = id;
    }
  } else {
    Serial.println("bbbbbbbbbb");
    leaderId = id;
    isLeader = true;
  }
  Serial.println(leaderId);
}

