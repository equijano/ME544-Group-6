//@@ -0,0 +1,228 @@
// Same base code as Ch_4_PID_LIDAR_longerdelay_center_inlet_double

#include <Servo.h>
#include <I2C.h>
//#include <Printers.h>
//#include <XBee.h>
#include <SoftwareSerial.h>
#define    LIDARLite_ADDRESS     0x62          // Default I2C Address of LIDAR-Lite.
#define    RegisterMeasure       0x00          // Register to write to initiate ranging.
#define    MeasureValue          0x04          // Value to initiate ranging.
#define    RegisterHighLowB      0x8f          // Register to get both High and Low bytes in 1 call.

int sensorPins[] = {4,5}; // Array of pins connected to the sensor Power Enable lines
int sensorPinsArraySize = 2; // The length of the array

int sensorPin1 = A0; //analog pin 0
int sensorPin2 = A3; //analog pin 0
  

//Car constants
Servo wheels; // servo for turning the wheels
Servo esc; // not actually a servo, but controlled like one!
bool startup = true; // used to ensure startup only happens once
int startupDelay = 1000; // time to pause at each calibration step
double maxSpeedOffset = 45; // maximum speed magnitude, in servo 'degrees'
double maxWheelOffset = 85; // maximum wheel turn magnitude, in servo 'degrees'

//PID constants
int errorPre = 0;
int errorSum = 0;

// //RSSI constants
// // Should be a unique ID between 0 and 255.
//uint8_t BEACON_ID = 4;
//int location = 0;
//
//XBee xbee = XBee();
//XBeeResponse response = XBeeResponse();
// //create reusable response objects for responses we expect to handle 
//ZBRxResponse rx = ZBRxResponse();
//ModemStatusResponse msr = ModemStatusResponse();
//
//uint8_t dbCommand[] = {'D','B'};
//AtCommandRequest atRequest = AtCommandRequest(dbCommand);
//
//ZBTxStatusResponse txStatus = ZBTxStatusResponse();
//AtCommandResponse atResponse = AtCommandResponse();
//
//SoftwareSerial xbeeSerial(2,3);

void setup(){
  Serial.begin(9600);
  
//  //xbee setup
//  xbeeSerial.begin(9600);
//  xbee.setSerial(xbeeSerial);
//  Serial.println("Initializing beacon...");
//  
//  Serial.println("Setup Command");
  wheels.attach(8); // initialize wheel servo to Digital IO Pin #8
  esc.attach(7); // initialize ESC to Digital IO Pin #9
  
  /*  If you're re-uploading code via USB while leaving the ESC powered on, 
   *  you don't need to re-calibrate each time, and you can comment this part out.
   */
  pinMode(A0,INPUT);
  calibrateESC();
//  Serial.println(" ESC just calibrated");
  esc.write(80);    
  I2c.begin(); // Opens & joins the irc bus as master
  delay(100); // Waits to make sure everything is powered up before sending or receiving data  
  I2c.timeOut(50); // Sets a timeout to ensure no locking up of sketch if I2C communication fails
  
  for (int i = 0; i < sensorPinsArraySize; i++){
    pinMode(sensorPins[i], OUTPUT); // Pin to first LIDAR-Lite Power Enable line
    Serial.print(sensorPins[i]);
  }
}





// //RSSI Functions //
//int sendATCommand(AtCommandRequest atRequest) {
//  int value = -1;
//  Serial.println("Sending command to the XBee");
//
//  // send the command
//  xbee.send(atRequest);
//
//  // wait up to 5 seconds for the status response
//  if (xbee.readPacket(5000)) {
//    // got a response!
//
//    // should be an AT command responses
//    if (xbee.getResponse().getApiId() == AT_COMMAND_RESPONSE) {
//      xbee.getResponse().getAtCommandResponse(atResponse);
//
//      if (atResponse.isOk()) {
//        Serial.print("Command [");
//        Serial.print(atResponse.getCommand()[0]);
//        Serial.print(atResponse.getCommand()[1]);
//        Serial.println("] was successful!");
//
//        if (atResponse.getValueLength() > 0) {
//          Serial.print("Command value length is ");
//          Serial.println(atResponse.getValueLength(), DEC);
//
//          Serial.print("Command value: ");
//          
//          for (int i = 0; i < atResponse.getValueLength(); i++) {
//            value = atResponse.getValue()[i];
//            Serial.print(atResponse.getValue()[i]);
//            Serial.print(" ");
//          }
//
//          Serial.println("");
//        }
//      } 
//      else {
//        Serial.print("Command return error code: ");
//        Serial.println(atResponse.getStatus(), HEX);
//      }
//    } else {
//      Serial.print("Expected AT response but got ");
//      Serial.print(xbee.getResponse().getApiId(), HEX);
//    }   
//  } else {
//    // at command failed
//    if (xbee.getResponse().isError()) {
//      Serial.print("Error reading packet.  Error code: ");  
//      Serial.println(xbee.getResponse().getErrorCode());
//    } 
//    else {
//      Serial.print("No response from radio");  
//    }
//  }
//  return value;
//}
//
//void sendRSSIValue(XBeeAddress64 targetAddress, int rssiValue){
//  uint8_t value = (uint8_t) rssiValue;
//  uint8_t values[] = {value};
//  //uint8_t values[] = {value, BEACON_ID};
//  ZBTxRequest zbTx = ZBTxRequest(targetAddress, values, sizeof(values));
//  sendTx(zbTx);
//}
//
//void sendTx(ZBTxRequest zbTx){
//  xbee.send(zbTx);
//
//   if (xbee.readPacket(500)) {
//    Serial.println("Got a response!");
//    // got a response!
//
//    // should be a znet tx status              
//    if (xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
//      xbee.getResponse().getZBTxStatusResponse(txStatus);
//
//      // get the delivery status, the fifth byte
//      if (txStatus.getDeliveryStatus() == SUCCESS) {
//        // success.  time to celebrate
//        Serial.println("SUCCESS!");
//      } else {
//        Serial.println("FAILURE!");
//        // the remote XBee did not receive our packet. is it powered on?
//      }
//    }
//  } else if (xbee.getResponse().isError()) {
//    Serial.print("Error reading packet.  Error code: ");  
//    Serial.println(xbee.getResponse().getErrorCode());
//  } else {
//    // local XBee did not provide a timely TX Status Response -- should not happen
//    Serial.println("This should never happen...");
//  }
//}
//
//int processResponse(int data){
//  int clocation;
//  if (xbee.getResponse().isAvailable()) {
//      // got something
//           
//      if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
//        // got a zb rx packet
//        
//        // now fill our zb rx class
//        xbee.getResponse().getZBRxResponse(rx);
//      
//        Serial.println("Got an rx packet!");
//            
//        if (rx.getOption() == ZB_PACKET_ACKNOWLEDGED) {
//            // the sender got an ACK
//            Serial.println("packet acknowledged");
//        } else {
//          Serial.println("packet not acknowledged");
//        }
//        
//        /*Serial.print("checksum is ");
//        Serial.println(rx.getChecksum(), HEX);
//        Serial.print("packet length is ");
//        Serial.println(rx.getPacketLength(), DEC);*/
//
//         for (int i = 0; i < rx.getDataLength(); i++) {
//          clocation += rx.getData()[i]; 
//          /*Serial.print("payload [");
//          Serial.print(i, DEC);
//          Serial.print("] is ");
//          Serial.println(rx.getData()[i], HEX);*/
//        }
//        /** IMPORTANT **/
//        /*Serial.print("Location: ");
//        Serial.println(location);
//        location = 0;*/
//       /*for (int i = 0; i < xbee.getResponse().getFrameDataLength(); i++) {
//        Serial.print("frame data [");
//        Serial.print(i, DEC);
//        Serial.print("] is ");
//        Serial.println(xbee.getResponse().getFrameData()[i], HEX);
//      }*/
//
//            
//      XBeeAddress64 replyAddress = rx.getRemoteAddress64();
//      int rssi;
//      if(data == 1){
//         rssi = sendATCommand(dbCommand);
//      } else {
//         /** IMPORTANT **/
//         rssi = data;
//      }
//      sendRSSIValue(replyAddress, rssi);
//      Serial.println("");
//        
//      }
//    } else if (xbee.getResponse().isError()) {
//      Serial.print("error code:");
//      Serial.println(xbee.getResponse().getErrorCode());
//    }
//    return clocation;
//}




/* Convert degree value to radians */
double degToRad(double degrees){
  return (degrees * 71) / 4068;
}

/* Convert radian value to degrees */
double radToDeg(double radians){
  return (radians * 4068) / 71;
}

/* Calibrate the ESC by sending a high signal, then a low, then middle.*/
void calibrateESC(){
    Serial.println("Entered calibrateESC function");
    esc.write(180); // full backwards
    delay(startupDelay);
    esc.write(0); // full forwards
    delay(startupDelay);
    esc.write(90); // neutral
    delay(startupDelay);
    esc.write(90); // reset the ESC to neutral (non-moving) value
}

void pTurn(){
  //for (int i =0; i < 180; i++){
    
  enableDisableSensor(4); // Turn on sensor attached to pin 2 and disable all others
  int val1 = readDistance();
  enableDisableSensor(5); // Turn on sensor attached to pin 2 and disable all others
  int val2 = readDistance();
  int average = (val1 + val2)/2;
  int middle_hall = 90;
  
  Serial.print("Val1: ");
  Serial.print(val1);
  Serial.print("  Val2: ");
  Serial.println(val2);
  
  if (val1>200 && val2>200){
     middle_hall = 725;
  }
  
    if (val1>5000 && val2>5000){  //added for door/error avoidance
     middle_hall = average - 90;
  }
  
  int error_p = middle_hall - average; //setpoint (center of hallway) = 60cm. tries to correct towards center of hallway
  int error_a = val1 - val2;
  int errorTotal = error_p + error_a;
  
//  Serial.print("sensorA: ");
//  Serial.println(val1);
//  
//  Serial.print("SensorB: ");
//  Serial.println(val2);
//  
//  Serial.print("Error: ");
  //Serial.println(error);
  
  int kpp = -2;  
  int kpa = -3;
  int kd = -.1;
  int ki = 0; //normally works well at -.05
  
  int ip= kpp*error_p;
  int ia = kpa*error_a - 50;
  
  int deltaError = errorTotal - errorPre;
  int id= (kd*deltaError);
  
  int errorSum = errorSum + errorTotal;
  int ii= ki*errorSum;
  
  int i = ip + ia + id +ii;
  
  
  if(i>90){
   i = 90; 
  }
  
  if(i<-90){
   i = -90; 
  }
//   
//   if(val1>200 && val2>200){
//   i = -5;
//   }
   
   
   if(val1>200 && val2>860){
   i = -5;
   TurnLeft();
   }
   if(val1>860 && val2>200){
   i = -5;
   TurnLeft();
   }
   
   if(val1==0 && val2==0){
   i = -5;
   TurnLeft();
   }
   if(val1==0 && val2>1000){
   i = -5;
   TurnLeft();
   }
   if(val1>1000 && val2==0){
   i = -5;
   TurnLeft();
   }
  
  //i = 270;
  
    double rad = degToRad(i);
    double speedOffset = sin(rad) * maxSpeedOffset;
    double wheelOffset = sin(rad) * maxWheelOffset;
    
    //esc.write(80);
    //esc.write(speedOffset);
    wheels.write(90 + wheelOffset);
    //delay(50);
    errorPre = errorTotal;
 // }
}



void enableDisableSensor(int sensorPin){
  for (int i = 0; i < sensorPinsArraySize; i++){
      digitalWrite(sensorPins[i], LOW); // Turn off all sensors
  }
  digitalWrite(sensorPin, HIGH); // Turn on the selected sensor
  delay(20); // The sensor takes 1msec to wake
}

int readDistance(){
  uint8_t nackack = 100; // Setup variable to hold ACK/NACK resopnses     
  while (nackack != 0){ // While NACK keep going (i.e. continue polling until sucess message (ACK) is received )
    nackack = I2c.write(LIDARLite_ADDRESS,RegisterMeasure, MeasureValue); // Write 0x04 to 0x00
    delay(20); // Wait 1 ms to prevent overpolling
  }

  byte distanceArray[2]; // array to store distance bytes from read function
  
  // Read 2byte distance from register 0x8f
  nackack = 100; // Setup variable to hold ACK/NACK resopnses     
  while (nackack != 0){ // While NACK keep going (i.e. continue polling until sucess message (ACK) is received )
    nackack = I2c.read(LIDARLite_ADDRESS,RegisterHighLowB, 2, distanceArray); // Read 2 Bytes from LIDAR-Lite Address and store in array
    delay(20); // Wait 1 ms to prevent overpolling
  }
  int distance = (distanceArray[0] << 8) + distanceArray[1];  // Shift high byte [0] 8 to the left and add low byte [1] to create 16-bit int
  
  return distance;   // Print Sensor Name & Distance
   
}


void TurnLeft(){
  Serial.println("TurnLeft command");
  esc.write(90); //stops the car
  int turndelay = 5000;
  
  // Back up with wheels turned right
  int i = -90;
  double rad = degToRad(i);
  double wheelOffset = sin(rad) * maxWheelOffset;
  wheels.write(90 + wheelOffset);
  esc.write(100); //drive reverse
  delay(turndelay);
  
  esc.write(90); //stops the car
  
  // Drive forward with wheels turned left
  i = 90;
  rad = degToRad(i);
  wheelOffset = sin(rad) * maxWheelOffset;
  wheels.write(90 + wheelOffset);
  esc.write(80); //drive forward
  int Sonar = 0;
  for (int j = 0; j<10; j++){
    delay(turndelay/10);
    Sonar = analogRead(A0);
    if(Sonar < 35){
      BackUp();
    }
  }
  
   i = 0;
  rad = degToRad(i);
  wheelOffset = sin(rad) * maxWheelOffset;
  wheels.write(90 + wheelOffset);
  delay(1000);
  
}



void BackUp(){
  Serial.println("BackUp Command");
  esc.write(90); //stops the car
  int turndelay = 5000; //amount of time car will back up (ms)
  
  // Back up with wheels turned how they were before
  esc.write(100); //drive reverse
  // Back up with wheels turned right
  int i = -90;
  double rad = degToRad(i);
  double wheelOffset = sin(rad) * maxWheelOffset;
  wheels.write(90 + wheelOffset);
  esc.write(100); //drive reverse
  delay(turndelay);
  
}

void manual(){
char A = 'z';
esc.write(90);
while (A == 'z'){
  if (Serial.available() > 0) {
    // esc.write(75);
     char s= Serial.read();    
    if(s == 'a'){
      wheels.write(150);
      Serial.println("Turing left");
      }
      
    if(s == 'd'){
      wheels.write(45);
      Serial.println("Turing right");
      }
      
      if(s == 'w'){
        esc.write(80);
      wheels.write(90);
      Serial.println("Go ahead");
      }

      if(s == 'e'){
        //esc.write(80);
      wheels.write(90);
      Serial.println("forward!");
      }
      
      if(s == 's'){
      esc.write(90);
      wheels.write(90);
      Serial.println("Stop");
      }

      if(s == 'x'){
      esc.write(100);
      wheels.write(90);
      Serial.println("Backwards");
      }

      if(s == 'q'){
      esc.write(90);
      A = 'q';
      Serial.println("Exit Manual");
      }
 
}

}
}




 
void loop(){ 
//  delay(500);
if (Serial.available() > 0) {
    // esc.write(75);
     char s= Serial.read();    
     if (s == 'p'){
      manual();
     }
  }else{

    esc.write(80);
    pTurn();
    }
  int Sonar = analogRead(A0);
  
  
  if (Sonar < 35){
   BackUp(); 
  }

    
//  if(location != 0){
//    Serial.print("location");
//    Serial.println(location);
//    location = 0;
//  }
//  Serial.print("Sonar: ");
//  Serial.println(Sonar); // Add line return
}

