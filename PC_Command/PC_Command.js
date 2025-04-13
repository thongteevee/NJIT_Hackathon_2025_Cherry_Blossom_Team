const { SerialPort, ReadlineParser } = require('serialport');
const { exec } = require('child_process');

const COM_PORT = 'COM7';
const BAUD_RATE = 115200;

const AHK_SCRIPT_PATH = '"M:\\HACKATHON\\PC_Command\\PC_Command.ahk"';

const port = new SerialPort({ path: COM_PORT, baudRate: BAUD_RATE });
const parser = new ReadlineParser();

port.pipe(parser);

parser.on("data", (line) => {
    console.log("ESP32 says:", line);
    if (line.includes("[MOTION]")) {
        console.log("⚡ Triggering AutoHotKey...");
        exec(AHK_SCRIPT_PATH, (error, stdout, stderr) => {
            if (error) {
                console.error("AHK Error:", error.message);
            }
            if (stderr) {
                console.error("AHK stderr:", stderr);
            }
            if (stdout) {
                console.log("AHK stdout:", stdout);
            }
        });
    }
});

// credit code to Tech Joyce