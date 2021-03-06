#include <Servo.h>

 int sensorPin1 = A0; //analog pin 0
  int sensorPin2 = A3; //analog pin 0
 
Servo wheels; // servo for turning the wheels
Servo esc; // not actually a servo, but controlled like one!
bool startup = true; // used to ensure startup only happens once
int startupDelay = 1000; // time to pause at each calibration step
double maxSpeedOffset = 45; // maximum speed magnitude, in servo 'degrees'
double maxWheelOffset = 
85; // maximum wheel turn magnitude, in servo 'degrees'

void setup()
{
  Serial.begin(9600);
  wheels.attach(8); // initialize wheel servo to Digital IO Pin #8
  esc.attach(9); // initialize ESC to Digital IO Pin #9
  
  /*  If you're re-uploading code via USB while leaving the ESC powered on, 
   *  you don't need to re-calibrate each time, and you can comment this part out.
   */
  calibrateESC();
  //esc.write(80);
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

/* Oscillate between various servo/ESC states, using a sine wave to gradually 
 *  change speed and turn values.
 */
void oscillateleft(){
  //for (int i =180; i < 360; i++){
    int i=170;
    double rad = degToRad(i);
   // double speedOffset = sin(rad) * maxSpeedOffset;
    double speedOffset = 15.0;
    double wheelOffset = sin(rad) * maxWheelOffset;
    esc.write(65 + speedOffset);
   // Serial.println(65 + speedOffset);
    //esc.write(100);
    //wheels.write(90 + wheelOffset);
    wheels.write(wheelOffset);
    delay(20);
  //}
}

void oscillatecenter(){
  //for (int i =180; i < 360; i++){
    int i=60;
    double rad = degToRad(i);
   // double speedOffset = sin(rad) * maxSpeedOffset;
    double speedOffset = 15.0;
    double wheelOffset = sin(rad) * maxWheelOffset;
    esc.write(65 + speedOffset);
   // Serial.println(65 + speedOffset);
    //esc.write(100);
    //wheels.write(90 + wheelOffset);
    wheels.write(wheelOffset);
    //delay(50);
  //}
}
void oscillateright(){
  //for (int i =0; i < 180; i++){
  int i=50;
    double rad = degToRad(i);
    double speedOffset = sin(rad) * maxSpeedOffset;
    double wheelOffset = sin(rad) * maxWheelOffset;
    esc.write(80);
    //esc.write(speedOffset);
    wheels.write(90 + wheelOffset);
    //delay(50);
 // }
}

int medianDistance(int pinNumber){
  int Distances[] = {0,0,0,0,0,0};
  int array_len = 6;
  for (int i = 0; i < array_len; i++){
    Distances[i] = analogRead(pinNumber);;
   }
  
  int index = 0;
  for (int i = 0; i < array_len/2; i++){
    int high = 0;
    index = 0;
   for (int j = 0; j < array_len; j++){
    if (Distances[j] > high){
      high = Distances[j];
      index = j;
    }
   }
   if (i == array_len/2 - 1){
     return high;
   }
   Distances[index] = 0; 
  }  
}


void loop()
{ 
   int val1 = medianDistance(sensorPin1);
   int val2 = medianDistance(sensorPin2);
  esc.write(75);
  
  Serial.print("a ");
  Serial.println(val1);

  
  Serial.print("b ");
  Serial.println(val2);
  Serial.print("minus");
  Serial.println(val1-val2);
  
  
  delay(80);
  if((val1-val2)>20){
    oscillateright();
    //delay(50);
    }
   //if((val1-val2)<20||(val2-val1)<20){
    //oscillatecenter();
   //esc.write(75);
    //delay(50);
    //}
    if((val2-val1)>15){
    oscillateleft();
     //delay(50); 
      }
      //delay(100);
    // esc.write(70);
   //oscillate();
   if(val1<5){
      oscillatecenter();    
   }
}

