const fs = require("fs");
const dgram = require('dgram');  // データグラム通信用のモジュールを読み込む
const recorder = require('node-record-lpcm16'); 

const socket = dgram.createSocket('udp4'); // IPv4のUDPソケットを作成

recorder
    .record({
        sampleRate: 48000, 
        channels: 1, 
        recordProgram: 'rec', 
    })
    .stream()
    .on('error', console.error) 
    .on('data', function(data) { 
        var vol = Math.abs(data.readInt16LE());  
        var ntimes = vol / 512;
        var bar = "*".repeat(ntimes); 
        console.log("mic volume:", bar);
        fs.appendFile("rec_saved.lpcm16", data, function(err){});
    });
console.log('Listening, press Ctrl+C to stop.');

