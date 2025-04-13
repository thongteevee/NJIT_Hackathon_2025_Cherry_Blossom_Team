const { SerialPort, ReadlineParser } = require('serialport');
const { exec } = require('child_process');
const notifier = require('node-notifier');
const path = require('path');

const COM_PORT = 'COM7';
const BAUD_RATE = 115200;

const port = new SerialPort({ path: COM_PORT, baudRate: BAUD_RATE });
const parser = new ReadlineParser();
port.pipe(parser);

parser.on("data", (line) => {
    const trimmedLine = line.trim();
    console.log("Sensor says:", trimmedLine);

    if (trimmedLine.includes("[MOTION]")) {

        exec('"M:\\HACKATHON\\PC_Command\\PC_Command.ahk"');

        notifier.notify({
            title: '🚨 Motion Detected',
            message: 'Someone triggered the ESP32 sensor!',
            icon: path.join("C:\\Users\\Thong Khong\\Downloads", "your-image.jpg"), // replace with your image path and name
            sound: true,
        });
    }
});
