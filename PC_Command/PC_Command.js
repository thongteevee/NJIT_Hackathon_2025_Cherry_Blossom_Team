const { SerialPort, ReadlineParser } = require('serialport');
const { exec } = require('child_process');
const notifier = require('node-notifier');
const path = require('path');

const COM_PORT = 'COM7';
const BAUD_RATE = 115200;

const port = new SerialPort({ path: COM_PORT, baudRate: BAUD_RATE });
const parser = new ReadlineParser();
port.pipe(parser);

let motionHandled = false; // KEEP ONLY FOR THE VIDEO

parser.on("data", (line) => {
    const trimmedLine = line.trim();
    console.log("Sensor says:", trimmedLine);

    if (trimmedLine.includes("[MOTION]") && !motionHandled) {       //KEEP ONLY FOR THE VIDEO
        motionHandled = true;       // KEEP ONLY FOR THE VIDEO

        // ✅ Run the AHK script
        exec('"M:\\HACKATHON\\PC_Command\\PC_Command.ahk"');

        // ✅ Show Windows notification
        notifier.notify({
            title: '🚨 Motion Detected',
            message: 'Someone triggered the ESP32 sensor!',
            icon: path.join("C:\\Users\\Thong Khong\\Downloads", "your-image.jpg"), // Use .ico or .png for best result
            sound: true,
        });
    }
});
