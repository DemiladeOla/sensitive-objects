const http = require("http");
const fs = require("fs");
const serialPort = require("serialport");

const index = fs.readFileSync("index.html");

// A parser to parse the information from the serial port into something we can work with
const parsers = serialPort.parsers;
const parser = new parsers.Readline({delimiter: "\r\n"});

// Sets a Serial stream reader on the serial port below
// Replace COM3 with your Arduino serial port id
const port = new serialPort("/dev/cu.usbmodem144201", {
    baudRate: 9600,
    dataBits: 8,
    parity: "none",
    stopBits: 1,
    flowControl: false
});

// You know what this one does
const app = http.createServer(function (req, res) {
    if(req.url === "/1.mp3") {
        let file = fs.readFileSync("1.mp3");
        res.writeHead(200, {"content-type": "audio/mpeg"});
        res.end(file);
    }

    if(req.url === "/") {
        res.writeHead(200, {"content-type": "text/html"});
        res.end(index);
    }
});

// Listen to a connection from the same stream as the already created app object
const io = require("socket.io")(app);

io.on("connection", function (data) {
    // Informs node that a client has connected to it using Socket.io
    console.log("NodeJS is listening...");
});

// Pipes the information from the serial reader to the serial to be converted to
// format we can work with
port.pipe(parser);
parser.on("data", function (data) {
    // Emit the value from the Arduino to the client using Socket.io
    io.emit("data", data);
});

app.listen(3000, function () {
    console.log(`Listening on port 3000...`);
});