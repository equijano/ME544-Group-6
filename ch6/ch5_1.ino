#include <XBee.h>
#include <math.h>
#include <SoftwareSerial.h>

SoftwareSerial xbee(2,3); // Rx, Tx

int red = 4;
int green = 5;
int blue = 6;

//current node
int identity;
int status = 0;
int leaderID;
boolean isLeader;

// Looping Variables
int temp_id;


//Counting timer
int checkLeaderTimer = 0;
int electionTimer = 0;
int leaderTimer = 0;

// Timeout variables
int timeout = 3;
int checkLeader_timeout = 9;
int leader_timeout = 1;
boolean timeout_flag = false;
int timeout_count = 0;

// Declare LED pin number


// input pin status
int switchState = 0;

//AT command get id
int getIdentity() {
  String s;

  // Enter configuration mode - Should return "OK" when successful.
  delay(1000);    // MUST BE 1000
  xbee.write("+++");
  delay(1000);    // MUST BE 1000
  xbee.write("\r");
  delay(100);

  // Get the OK and clear it out.
  while (xbee.available() > 0) {
    Serial.print(char(xbee.read()));
  }
  Serial.println("");

  // Send "ATNI" command to get the NI value of xBee.
  xbee.write("ATNI\r");
  delay(100);
  while (xbee.available() > 0) {
      s += char(xbee.read());
  }
  delay(100);

  // Exit configuration mode
  xbee.write("ATCN\r");
  delay(1000);

  // Flush Serial Buffer Before Start
  while (xbee.available() > 0) {
    Serial.print(char(xbee.read()));
  }
  Serial.println("");
  delay(100);

  return s.toInt();
}

String readMsg() {
  String msg  = "";
  while(xbee.available() > 0) {
    char c = char(xbee.read());
    // cut the msg by ‘\n’
    if (c == '\n') {
      break;
    }
    msg += c;
  }
  Serial.println(msg);
  return msg;
}

void infection(){
  // only affect None-leaders
  if(!isLeader){
    digitalWrite(red, HIGH);
    digitalWrite(green, LOW);
    // set the status to be 1 (infected)
    status = 1;
  }
}

void cure(){
  digitalWrite(red, LOW);
  digitalWrite(green, HIGH);
  status = 0;
}

/*
  Check whether the election is finished or not
*/
boolean checkelectionTimeOut() {
  if (timeout_flag) {
    if (timeout_count < 2) {
      timeout_count++;
    } else {
      timeout_flag = false;
      timeout_count = 0;
    }
  }
  return timeout_flag;
}

/*
  when a leader died or a new ardunio added, raise the election function
*/
void election(int id) {
  Serial.println("Electing...");
  digitalWrite(blue, LOW);
  if (checkelectionTimeOut()) {
    Serial.println("finish election");
    return;
  }
  leaderID = -1;
  if (id > temp_id) {
    // New candidate with higher ID came in
    temp_id = id;
    electionTimer = 0;
    broadcastMsg(temp_id);
  } else {
    if (electionTimer >= timeout){
      // Election is over
      electionTimer = 0;
      timeout_count = 0;
      timeout_flag = true;
      assignLeader();
      Serial.println("election has ran out of time");
    } else {
      electionTimer++;
      broadcastMsg(temp_id);
    }
  }
}

/*
 select the leader and also check out the status of current node
*/
void assignLeader() {
  leaderID = temp_id;
  temp_id = identity;
  Serial.println("Leader ID : "  + String(leaderID));
  if (leaderID == identity) {
    status = 0;
    digitalWrite(green, HIGH);
  } else {
    if (status == 1) {
      digitalWrite(red, HIGH);
      digitalWrite(green, LOW);
    } else {
      digitalWrite(green, HIGH);
      digitalWrite(red, LOW);
    }
  }
}
/*
  tell others the leader's ID and whether it is alive or not
*/
void broadcastMsg(int id) {
  xbee.print(String(id) + ":Leader:" + String(status) + "\n");
}

/*
  tell others leader is now alive
*/
void leaderBroadcast() {
  xbee.print(String(identity)+ ":Leader\n");
  Serial.println("The new leader is:" + String(leaderID));
  digitalWrite(blue, HIGH);
  digitalWrite(green, HIGH);
  digitalWrite(red, LOW);
  isLeader = true;
}


void infectOthers() {
  xbee.print(String(identity) + ":Infection\n");
  Serial.println("Spread disease");
  digitalWrite(red, HIGH);
  digitalWrite(green, LOW);
  status = 1;
}

void cureOthers() {
  xbee.print(String(identity) + ":Curing\n");
  status = 0;
  Serial.println("leader is sending the medicine.");
}

// check the input :
//    0 represents don't do anything.
//    1 represents send medicine or disease.
void checkStatus () {
  switchState = digitalRead(8);
  if(switchState == 1){
    if(isLeader){
      cureOthers();
    }else{
      infectOthers();
    }
  }
}

void setup() {
  xbee.begin(9600);
  Serial.begin(9600);

  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
  pinMode(red, OUTPUT);
  pinMode(8, INPUT);
  digitalWrite(green,HIGH);

  isLeader = false;
  identity = getIdentity();
  temp_id = identity;
  leaderID = -1;
  Serial.println("My ID is : "+ String(identity));
  xbee.flush();
}

void loop(){
  checkStatus();
  if (xbee.available()) {
    // Received MSG
    String msg = readMsg();
    String info = msg.substring(msg.indexOf(':') + 1);
    int id = msg.substring(0, msg.indexOf(':')).toInt();
    if (info == "Infection") {
      // Infection msg
      infection();
    } else if (info == "Curing") {
      // Cure msg
      cure();
      checkLeaderTimer = 0;
    } else {
      // Leader msg
      Serial.println(info);
      String othersStatus = info.substring(info.indexOf(':') + 1);
      info = info.substring(0, info.indexOf(':'));
      if (othersStatus == "1") {
        // Only update the status when someone is infected
        status = 1;
      }
      isLeader = false;
      checkLeaderTimer = 0;
      if (id == leaderID) {
        if (leaderID == identity) {
          leaderBroadcast();
        } else {
          // leader msg, sent from leader : leader is alive
          Serial.println("leader is exist");
        }
      } else {
        // leader msg, sent from None-leaders : election
        election(id);
      }
    }
  } else {
    // No msg received, check if leader is still alive
   if (leaderID == identity) {
    if (leaderTimer >= leader_timeout) {
      leaderTimer = 0;
      leaderBroadcast();
    } else {
      leaderTimer++;
    }
  } else if(checkLeaderTimer >= checkLeader_timeout){
        //fix the bug when remove the rest Arduino but leave one
        checkLeaderTimer = 0;
        leaderID = -1;
        broadcastMsg(identity);
        Serial.println("Leader "+String(leaderID) + "dead.");
    }else {
      if (leaderID == -1) {
        // leader is not assigned yet, election is not finished
        checkLeaderTimer = 0;
        if (electionTimer < timeout) {
          broadcastMsg(temp_id);
          electionTimer++;
        } else {
          electionTimer = 0;
          assignLeader();
        }
      } else {
         checkLeaderTimer++;
      }
    }
  }
  delay(1000);
}
