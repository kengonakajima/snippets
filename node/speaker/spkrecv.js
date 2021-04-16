const Readable=require("stream").Readable; 
const Speaker=require("speaker");
const dgram = require("dgram");


const receiver=new Readable();
receiver.buf = new Uint8Array(48000*2);
receiver.buf_used=0;
receiver._read = function(n) {
    var sampleNum = n / 2;
    var sampleInbuffer = this.buf_used / 2;
    console.log("sampleInbuffer:", sampleInbuffer, " sampleNum:",sampleNum);
    var outary = new Uint8Array(n);
    if(sampleInbuffer>sampleNum) {
        var outdv = new DataView(outary.buffer);
        var indv = new DataView(this.buf.buffer);
        for(var i=0;i<sampleNum;i++) {
            var sample=indv.getInt16(i*2,true);
            outdv.setInt16(i*2,sample,true);
        }
        for(var i=n;i<this.buf_used;i++) {
            this.buf[i-n]=this.buf[i];
        }
        console.log("played",sampleNum);
        this.buf_used-=n;
    } else {
        for(var i=0;i<n;i++) {
            outary[i]=0;
        }
    }
        
    this.push(outary);
}

const socket = dgram.createSocket("udp4");
socket.on("message", (message,remote) => {
    console.log("recv data:",remote.address,remote.port,message.length);        
    if(receiver.buf_used+message.length>receiver.buf.length) {
        console.log("buffer full");
        return;
    }
    for(var i=0;i<message.length;i++) {
        receiver.buf[receiver.buf_used+i]=message[i];
    }
    receiver.buf_used+=message.length;
});

socket.bind(23456,"0.0.0.0");


const spk=new Speaker({ 
    channels: 1, // チャンネル数は1(モノラル)
    bitDepth: 16, // サンプリングデータのビット数は16 (デフォルトはリトルエンディアン)
    sampleRate: 48000, // サンプリングレート(Hz)
});

receiver.pipe(spk); 

