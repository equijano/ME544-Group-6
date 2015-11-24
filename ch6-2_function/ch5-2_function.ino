void checkLeader() {
  if (leaderID == identity.toInt()) {
    if (leader_timer == leader_timeout) {
      leader_timer = 0;
      leaderBroadcast();
    } else {
      leader_timer++;
    }
  } else if(checkLeaderExpire){
    //fix the bug when remove the rest Arduino but leave one
    checkLeader_timer = 0;
    broadcastMsg(identity);
  } else {
   checkLeader_timer++;
  }
}

void setup (){
  Serial.begin(BAUD);
  xbeeSerial.begin(BAUD);
  xbee.setSerial(xbeeSerial);
  Serial.println("Initializing transmitter...");
}

//rebroadcast leader id
void broadcastMsg(int w) {
  uint8_t value = (uint8_t)id;
  uint8_t payload[] = {value};
  ZBTxRequest zbTx = ZBTxRequest(broadcastAddr, payload, sizeof(payload));
  xbee.send(zbTx);
}

void leaderBroadcast() {
  uint8_t payload[] = {identity};
  ZBTxRequest zbTx = ZBTxRequest(broadcastAddr, payload, sizeof(payload));
  xbee.send(zbTx);
}

boolean checkLeaderExpire() {
  if (checkLeader_timer >= checkLeader_timeout || leaderID == -1) {
    leaderID = -1;
    return true;
  } else {
    return false;
  }
}

boolean checkElectionTimeOut() {
  if (timeout_flag) {
    if (timeout_count < 3) {
      timeout_count++;
    } else {
      timeout_flag = false;
      timeout_count = 0;
    }
  }
  return timeout_flag;
}

