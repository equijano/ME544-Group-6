var SerialPort = require("serialport");
var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);

var portName = process.argv[2],
portConfig = {
	baudRate: 9600,
	parser: SerialPort.parsers.readline("\n")
};

var sp;

sp = new SerialPort.SerialPort(portName, portConfig);

app.get('/', function(req, res){
  res.sendfile('index.html');
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
var sumData = 0;
var count = 0;
var max = 4;
sp.on("open", function () {
  console.log('open');
  sp.on('data', function(data) {
    console.log('data received: ' + data);
    //io.emit("chat message", "An XBee says: " + data);
    io.emit("chat message", data);
    var temp = data.slice(7);
    count++;
    if(Number(temp) < -100 || Number(temp) > 100){
      max--;
    } else{
      sumData += Number(temp);
    }
    //io.emit("chat message", "An XBee says: " + temp);
    if(count%4 == 0){
      io.emit("chat message", "The average temperature is: " + ((Number(sumData))/(Number(max))).toFixed(2));
      sumData = 0;
      count = 0;
      max = 4;
    }
  });
});

