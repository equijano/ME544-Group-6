var SerialPort = require("serialport");
var express = require('express');
var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);
var array = ["null", "null", "null", "null"];
var fs = require('fs');
app.use(express.static('public'));
var portName = process.argv[2],
portConfig = {
	baudRate: 9600,
	parser: SerialPort.parsers.readline("\n")
};

var sp;

sp = new SerialPort.SerialPort(portName, portConfig);

app.get('/', function(req, res){
  res.sendFile(__dirname + '/index.html');
});

app.get('/history', function(req, res){
  res.sendFile(__dirname + '/history.html');
});
io.on('connection', function(socket){
  console.log('a user connected');
  socket.on('disconnect', function(){
  });
  socket.on('chat message', function(msg){
    io.emit('chat message', msg);
    sp.write(msg + "\n");
  });
});

http.listen(3000, function(){
  console.log('listening on *:3000');
});

sp.on("open", function () {
  console.log('open');
  sp.on('data', function(data) {
    console.log('data received: ' + data);
    //io.emit("chat message", "An XBee says: " + data);
    io.emit("chat message", data);
    if(data.indexOf("A") == 0){
      var d = Date.now();
      array[0] = data.slice(7);
      updateData(Number(data.slice(7)), d, "0")
    }
    else if(data.indexOf("B") == 0){
      array[1] = data.slice(7);
      var d = Date.now();
      updateData(Number(data.slice(7)), d, "1")
    }
    else if(data.indexOf("C") == 0){
      array[2] = data.slice(7);
      var d = Date.now();
      updateData(Number(data.slice(7)), d, "2")
    }
    else if(data.indexOf("D") == 0){
      array[3] = data.slice(7);  
      var d = Date.now();
      updateData(Number(data.slice(7)), d, "3")
    }
    if(array.indexOf("null") == -1){
      sumData = Number(array[0]) + Number(array[1]) + Number(array[2]) + Number(array[3]);
      io.emit('chat message', "V Temp:" + Number(sumData/4));
      array = ["null", "null", "null", "null"];
      var d = Date.now();
      updateData((Number(sumData/4)), d, "4");
    }
  });
});

function updateData(message, date, intId){
  var newData = {
    "newData": {
        "unit" : "" + date,
        "value" : ""+ message
      },
  };
  var obj = require(__dirname + "/data.json");
  var chart = obj["JSChart"];
  var dataset = (chart["datasets"]);
  var tmp = dataset[intId].data;
  tmp.push(newData["newData"]);
  var writeData = obj;
  var writeStream = fs.createWriteStream(__dirname + "/data.json");
  writeStream.write(JSON.stringify(writeData));
  writeStream.end();
}

http.listen(3000, function(){
  console.log('listening on *:3000');
});
