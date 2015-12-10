//@@ -0,0 +1,228 @@
// Same base code as Ch_4_PID_LIDAR_longerdelay_center_inlet_double

#include <SoftwareSerial.h>
#include <Servo.h>
#include <I2C.h>
#define    LIDARLite_ADDRESS     0x62          // Default I2C Address of LIDAR-Lite.
#define    RegisterMeasure       0x00          // Register to write to initiate ranging.
#define    MeasureValue          0x04          // Value to initiate ranging.
#define    RegisterHighLowB      0x8f          // Register to get both High and Low bytes in 1 call.

int sensorPins[] = {4,5}; // Array of pins connected to the sensor Power Enable lines
int sensorPinsArraySize = 2; // The length of the array

 int sensorPin1 = A0; //analog pin 0
  int sensorPin2 = A3; //analog pin 0
  
 
Servo wheels; // servo for turning the wheels
Servo esc; // not actually a servo, but controlled like one!
bool startup = true; // used to ensure startup only happens once
int startupDelay = 1000; // time to pause at each calibration step
double maxSpeedOffset = 45; // maximum speed magnitude, in servo 'degrees'
double maxWheelOffset = 85; // maximum wheel turn magnitude, in servo 'degrees'

int errorPre = 0;
int errorSum = 0;

SoftwareSerial XBee(2, 3); // RX, TX

void setup(){
  //XBee.begin(9600);
  Serial.begin(9600);
  wheels.attach(8); // initialize wheel servo to Digital IO Pin #8
  esc.attach(7); // initialize ESC to Digital IO Pin #9
  
  /*  If you're re-uploading code via USB while leaving the ESC powered on, 
   *  you don't need to re-calibrate each time, and you can comment this part out.
   */
  pinMode(A0,INPUT);
  calibrateESC();
  //esc.write(80);    
  I2c.begin(); // Opens & joins the irc bus as master
  delay(100); // Waits to make sure everything is powered up before sending or receiving data  
  I2c.timeOut(50); // Sets a timeout to ensure no locking up of sketch if I2C communication fails
  
  for (int i = 0; i < sensorPinsArraySize; i++){
    pinMode(sensorPins[i], OUTPUT); // Pin to first LIDAR-Lite Power Enable line
    Serial.print(sensorPins[i]);
  }
}

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
  int error_p = 80 - average; //setpoint (center of hallway) = 60cm. tries to correct towards center of hallway
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
  int ki = -.05;
  
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
   if(val1>200 && val2>200){
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
  esc.write(90); //stops the car
  int turndelay = 5000;
  
  // Back up with wheels turned how they were before
  esc.write(100); //drive reverse
  delay(turndelay);
  
}




 
void loop()
{ 
while (Serial.available() > 0) {
     esc.write(75);
     char s= Serial.read();
    XBee.write(s);    
    if(s == '1'){
      //state = "Wheels turning left.";
      //digitalWrite(sensorPin, LOW);
      wheels.write(150);
      XBee.write(" Turning left");
      Serial.println("Turing left");
      }
    if(s == '2'){
      //state = "Wheels turning left.";
      //digitalWrite(sensorPin, LOW);
      wheels.write(45);
      XBee.write(" Turning right");
      Serial.println("Turing right");
      }
  esc.write(75);
  pTurn();
  int Sonar = analogRead(A0);
  if (Sonar < 35){
   BackUp(); 
  }
  }
//  Serial.print("Sonar: ");
//  Serial.println(Sonar); // Add line return
}
