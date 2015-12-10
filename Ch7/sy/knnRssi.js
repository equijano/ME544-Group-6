var nn = require('nearest-neighbor');
var items = [
  { x: 87.70, y: 44.91, z: 80.23, p: 1 },
  { x: 82.90, y: 55.67, z: 68.49, p: 1 },
  { x: 69.53, y: 59.39, z: 68.01, p: 1 },
  { x: 63.21, y: 64.12, z: 59.14, p: 1 },
  { x: 60.79, y: 75.46, z: 46.30, p: 1 },
  { x: 59.42, y: 76.99, z: 47.88, p: 1 },
  { x: 51.16, y: 83.83, z: 48.00, p: 1 },
  { x: 47.50, y: 84.22, z: 52.06, p: 1 },
  { x: 48.22, y: 83.98, z: 58.23, p: 1 },
  { x: 54.25, y: 85.25, z: 68.00, p: 1 },
  { x: 60.31, y: 78.78, z: 83.40, p: 1 },
  { x: 72.14, y: 70.30, z: 85.00, p: 1 },
  { x: 71.49, y: 57.23, z: 75.00, p: 1 },
  { x: 85.86, y: 53.78, z: 84.51, p: 1 },
  { x: 86.03, y: 51.00, z: 84.00, p: 1 },
  { x: 83.00, y: 46.69, z: 83.29, p: 1 },
];

var fields = [
  { name: "x", measure: nn.comparisonMethods.number, max: 100 },
  { name: "y", measure: nn.comparisonMethods.number, max: 100 },
  { name: "z", measure: nn.comparisonMethods.number, max: 100 }, 
  { name: "p", measure: nn.comparisonMethods.number, max: 100 }, 
];

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
var location = "0";
var tmplocation = "0";
var sampleDelay = 2000;


//Note that with the XBeeAPI parser, the serialport's "data" event will not fire when messages are received!
portConfig = {
  baudRate: 9600,
  parser: XBeeAPI.rawParser()
};

var sp;
sp = new SerialPort.SerialPort(portName, portConfig);

var portName1 = process.argv[3],
portConfig1 = {
  baudRate: 9600,
  parser: SerialPort.parsers.readline("\n")
};

var sp1;
sp1 = new SerialPort.SerialPort(portName1, portConfig1);

var reading = ["null", "null", "null", "null"];
//var reading = [1, 1, 1];

var prop1 = ["null", "null"];
var prop2 = ["null", "null"];

XBeeAPI.on("frame_object", function(frame) {
  if (frame.type == 144 && frame.data[0] < 100){
    //console.log("Beacon ID: " + frame.data[1] + ", RSSI: " + (frame.data[0]));
    var id = String(frame.data[1]);
    if(id.indexOf("1") == 0){
      reading[0] = frame.data[0];
    }
    else if(id.indexOf("2") == 0){
      reading[1] = frame.data[0];
    }
    else if(id.indexOf("3") == 0){
      reading[2] = frame.data[0];
    }
    else if(id.indexOf("4") == 0){
      reading[3] = frame.data[0];
    }
  }
  if(reading.indexOf("null") == -1){
      var query = { x: reading[0], y: reading[1], z: reading[2], p: reading[3]};
      nn.findMostSimilar(query, items, fields, function(nearestNeighbor, probability) {
        //console.log(query);
        //console.log(nearestNeighbor);
        //console.log("Probablity: " + probability);
        for(var i = 0; i < items.length; i++){
          if((nearestNeighbor.x == items[i].x) && (nearestNeighbor.y == items[i].y) && (nearestNeighbor.z == items[i].z)){
            tmplocation = i + 1;
            break;
          }
        }
        //console.log("tmplocation: " + tmplocation);
        if(prop1.indexOf("null") == 0){
          prop1[0] = probability;
          prop1[1] = tmplocation;
        //} else if(prop2.indexOf("null") == 0){
        } else{
          prop2[0] = probability;
          prop2[1] = tmplocation;
        }
      });
      reading = ["null", "null", "null", "null"];
  }
  if((prop1.indexOf("null") == -1) && (prop2.indexOf("null") == -1)){
    //console.log(prop1[0] + " " + prop1[1]);
    //console.log(prop2[0] + " " + prop2[1]);
    if(((prop2[1] - prop1[1]) < 3) && ((prop2[1] - prop1[1]) > 0)){
      location = prop2[1];
    } else if(prop1[0] > prop2[0]){
      location = prop1[1];
    } else {
      location = prop2[1];
    }
    prop1 = ["null", "null"];
    prop2 = ["null", "null"];
    console.log("Final Location: " + location); 
  }
});

//Create a packet to be sent to all other XBEE units on the PAN.
// The value of 'data' is meaningless, for now.
var RSSIRequestPacket = {
  type: C.FRAME_TYPE.ZIGBEE_TRANSMIT_REQUEST,
  destination64: "000000000000ffff",
  broadcastRadius: 0x01,
  options: 0x00,
  data: location
}

var requestRSSI = function(){
  sp.write(XBeeAPI.buildFrame(RSSIRequestPacket));
  io.emit('chat message', location);
  sp.write(location + "\n");
}

io.on('connection', function(socket){
  socket.on('chat message', function(msg){
    //io.emit('chat message', msg);
    sp1.write(msg + "\n");
    console.log(msg);
  });
});

sp1.on("open", function () {
  console.log('open');
  sp1.on('data', function(data) {});
});

sp.on("open", function () {
  console.log('open');
  requestRSSI();
  setInterval(requestRSSI, sampleDelay);
});

app.get('/', function(req, res){
  res.sendfile('index.html');
});

http.listen(3000, function(){
  console.log('listening on *:3000');
});