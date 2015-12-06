var SerialPort = require("serialport");
var app = require('express')();
var xbee_api = require('xbee-api');
var http = require('http').Server(app);
var io = require('socket.io')(http);

var C = xbee_api.constants;
var XBeeAPI = new xbee_api.XBeeAPI({
  api_mode: 2
});

var portName = process.argv[2];

var sampleDelay = 3000;



//Note that with the XBeeAPI parser, the serialport's "data" event will not fire when messages are received!
portConfig = {
	baudRate: 9600,
  parser: XBeeAPI.rawParser()
};

var sp;
sp = new SerialPort.SerialPort(portName, portConfig);


//Create a packet to be sent to all other XBEE units on the PAN.
// The value of 'data' is meaningless, for now.
var RSSIRequestPacket = {
  type: C.FRAME_TYPE.ZIGBEE_TRANSMIT_REQUEST,
  destination64: "000000000000ffff",
  broadcastRadius: 0x01,
  options: 0x00,
  data: "test"
}

var requestRSSI = function(){
  sp.write(XBeeAPI.buildFrame(RSSIRequestPacket));
}

sp.on("open", function () {
  console.log('open');
  requestRSSI();
  setInterval(requestRSSI, sampleDelay);
});

var reading = ["null", "null", "null", "null"];

XBeeAPI.on("frame_object", function(frame) {
  if (frame.type == 144){
    //console.log("Beacon ID: " + frame.data[1] + ", RSSI: " + (frame.data[0]));
    if(frame.data[1].indexOf("1") == 0){
      reading[0] = frame.data[0];
    }
    else if(frame.data[1].indexOf("2") == 0){
      array[1] = frame.data[0];
    }
    else if(frame.data[1].indexOf("3") == 0){
      array[2] = frame.data[0];
    }
    else if(frame.data[1].indexOf("4") == 0){
      array[3] = frame.data[0];  
    }
  }
  if(reading.indexOf("null") == -1){
      var r = getPosition(reading, cor);
      console.log(r);
      array = ["null", "null", "null", "null"];
    }
});

app.get('/', function(req, res){
  res.sendfile('index.html');
});

http.listen(3000, function(){
  console.log('listening on *:3000');
});

io.on('connection', function(socket){
  //console.log('a user connected');
  /*socket.on('disconnect', function(){
  });*/
  socket.on('chat message', function(msg){
    io.emit('chat message', msg);
    sp.write(msg + "\n");
  });
});
/*var aa = [1, 2, 3, 4];
var ab = [1, 2, 3, 4];
var ac = [1, 2, 3, 4];
var ad = [1, 2, 3, 4];
var ba = [1, 2, 3, 4];
var bb = [1, 2, 3, 4];
var bc = [1, 2, 3, 4];
var bd = [1, 2, 3, 4];
var ca = [1, 2, 3, 4];
var cb = [1, 2, 3, 4];
var cc = [1, 2, 3, 4];
var cd = [1, 2, 3, 4];
var da = [1, 2, 3, 4];
var db = [1, 2, 3, 4];
var dc = [1, 2, 3, 4];
var dd = [1, 2, 3, 4];*/

var cor = [
  [1, 2, 3, 4],
  [1, 2, 3, 4],
  [1, 2, 3, 4],
  [1, 2, 3, 4],
  [1, 2, 3, 4],
  [1, 2, 3, 4],
  [1, 2, 3, 4],
  [1, 2, 3, 4],
  [1, 2, 3, 4],
  [1, 2, 3, 4],
  [1, 2, 3, 4],
  [1, 2, 3, 4],
  [1, 2, 3, 4],
  [1, 2, 3, 4],
  [1, 2, 3, 4],
  [1, 2, 3, 4],
];

function getDistance(a, b) {
  var sum = (a[0] - b[0]) * (a[0] - b[0]) + (a[1] - b[1]) * (a[1] - b[1]) + (a[2] - b[2]) * (a[2] - b[2]) + (a[3] - b[3]) * (a[3] - b[3]);
  return sum;
}

var result = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0];

function getPosition(reading, coor) {
  var length = ["100000", "null"];
  for(var i = 0; i < result.length(); i++){
    result[i] = getDistance(Number(reading), Number(cor[i]));
    if(result[i] < Number(length[0])){
      length[0] = result[i];
      length[1] = i;
    }
  }
  return length[1];
  /*var saa = getDistance(reading, aa);
  var sab = getDistance(reading, ab);
  var sac = getDistance(reading, ac);
  var sad = getDistance(reading, ad);
  var sba = getDistance(reading, ba);
  var sbb = getDistance(reading, bb);
  var sbc = getDistance(reading, bc);
  var sbd = getDistance(reading, bd);
  var sca = getDistance(reading, ca);
  var scb = getDistance(reading, cb);
  var scc = getDistance(reading, cc);
  var scd = getDistance(reading, cd);
  var sda = getDistance(reading, da);
  var sdb = getDistance(reading, db);
  var sdc = getDistance(reading, dc);
  var sdd = getDistance(reading, dd);*/
}

var SerialPort = require("serialport");
var app = require('express')();
var xbee_api = require('xbee-api');
var http = require('http').Server(app);
var io = require('socket.io')(http);

var C = xbee_api.constants;
var XBeeAPI = new xbee_api.XBeeAPI({
  api_mode: 2
});

var portName = process.argv[2];

var sampleDelay = 3000;



//Note that with the XBeeAPI parser, the serialport's "data" event will not fire when messages are received!
portConfig = {
  baudRate: 9600,
  parser: XBeeAPI.rawParser()
};

var sp;
sp = new SerialPort.SerialPort(portName, portConfig);


//Create a packet to be sent to all other XBEE units on the PAN.
// The value of 'data' is meaningless, for now.
var RSSIRequestPacket = {
  type: C.FRAME_TYPE.ZIGBEE_TRANSMIT_REQUEST,
  destination64: "000000000000ffff",
  broadcastRadius: 0x01,
  options: 0x00,
  data: "test"
}

var requestRSSI = function(){
  sp.write(XBeeAPI.buildFrame(RSSIRequestPacket));
}

sp.on("open", function () {
  console.log('open');
  requestRSSI();
  setInterval(requestRSSI, sampleDelay);
});

var reading = ["null", "null", "null", "null"];

XBeeAPI.on("frame_object", function(frame) {
  if (frame.type == 144){
    //console.log("Beacon ID: " + frame.data[1] + ", RSSI: " + (frame.data[0]));
    if(frame.data[1].indexOf("1") == 0){
      reading[0] = frame.data[0];
    }
    else if(frame.data[1].indexOf("2") == 0){
      array[1] = frame.data[0];
    }
    else if(frame.data[1].indexOf("3") == 0){
      array[2] = frame.data[0];
    }
    else if(frame.data[1].indexOf("4") == 0){
      array[3] = frame.data[0];  
    }
  }
  if(reading.indexOf("null") == -1){
      var r = getPosition(reading, cor);
      console.log(r);
      array = ["null", "null", "null", "null"];
    }
});

app.get('/', function(req, res){
  res.sendfile('index.html');
});

http.listen(3000, function(){
  console.log('listening on *:3000');
});

io.on('connection', function(socket){
  //console.log('a user connected');
  /*socket.on('disconnect', function(){
  });*/
  socket.on('chat message', function(msg){
    io.emit('chat message', msg);
    sp.write(msg + "\n");
  });
});
/*var aa = [1, 2, 3, 4];
var ab = [1, 2, 3, 4];
var ac = [1, 2, 3, 4];
var ad = [1, 2, 3, 4];
var ba = [1, 2, 3, 4];
var bb = [1, 2, 3, 4];
var bc = [1, 2, 3, 4];
var bd = [1, 2, 3, 4];
var ca = [1, 2, 3, 4];
var cb = [1, 2, 3, 4];
var cc = [1, 2, 3, 4];
var cd = [1, 2, 3, 4];
var da = [1, 2, 3, 4];
var db = [1, 2, 3, 4];
var dc = [1, 2, 3, 4];
var dd = [1, 2, 3, 4];*/

var cor = [
  [1, 2, 3, 4],
  [1, 2, 3, 4],
  [1, 2, 3, 4],
  [1, 2, 3, 4],
  [1, 2, 3, 4],
  [1, 2, 3, 4],
  [1, 2, 3, 4],
  [1, 2, 3, 4],
  [1, 2, 3, 4],
  [1, 2, 3, 4],
  [1, 2, 3, 4],
  [1, 2, 3, 4],
  [1, 2, 3, 4],
  [1, 2, 3, 4],
  [1, 2, 3, 4],
  [1, 2, 3, 4],
];

function getDistance(a, b) {
  var sum = (a[0] - b[0]) * (a[0] - b[0]) + (a[1] - b[1]) * (a[1] - b[1]) + (a[2] - b[2]) * (a[2] - b[2]) + (a[3] - b[3]) * (a[3] - b[3]);
  return sum;
}

var result = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0];

function getPosition(reading, coor) {
  var length = ["100000", "null"];
  for(var i = 0; i < result.length(); i++){
    result[i] = getDistance(Number(reading), Number(cor[i]));
    if(result[i] < Number(length[0])){
      length[0] = result[i];
      length[1] = i;
    }
  }
  return length[1];
  /*var saa = getDistance(reading, aa);
  var sab = getDistance(reading, ab);
  var sac = getDistance(reading, ac);
  var sad = getDistance(reading, ad);
  var sba = getDistance(reading, ba);
  var sbb = getDistance(reading, bb);
  var sbc = getDistance(reading, bc);
  var sbd = getDistance(reading, bd);
  var sca = getDistance(reading, ca);
  var scb = getDistance(reading, cb);
  var scc = getDistance(reading, cc);
  var scd = getDistance(reading, cd);
  var sda = getDistance(reading, da);
  var sdb = getDistance(reading, db);
  var sdc = getDistance(reading, dc);
  var sdd = getDistance(reading, dd);*/
}