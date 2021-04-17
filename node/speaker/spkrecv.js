const Readable=require("stream").Readable; 
const Speaker=require("speaker");
const dgram = require("dgram"); // データグラム通信用のモジュールを読み込む

const receiver=new Readable();
var recvbuf = new Uint8Array(48000*2); // 受信用のバッファを1秒分確保する(48KHzで1サンプルあたり2バイト)
var recvbuf_used=0; // 受信用のバッファに、有効なデータが何バイト入っているかを保持する変数
receiver._read = function(n) {
    var outary = new Uint8Array(n); // 出力用サンプル
    if(recvbuf_used>=n) { // 受信用バッファにあるサンプルが要求されたサンプル数以上の数ある場合
        for(var i=0;i<n;i++) outary[i]=recvbuf[i]; 
        for(var i=n;i<recvbuf_used;i++) recvbuf[i-n]=recvbuf[i]; // 受信用バッファをシフト(コピーし終わった分を捨てる)
        recvbuf_used-=n; // 捨てた分のバイト数だけ減算
    } else {
        for(var i=0;i<n;i++) outary[i]=0; // データが足りないときは無音を再生する
    }
    this.push(outary);
}

const socket = dgram.createSocket("udp4"); // IPv4のUDPを使うソケットを作る
socket.on("message", (message,remote) => { // UDPのデータグラムを受信した時のイベント
    console.log("recv data:",remote.address,remote.port,message.length);        
    if(recvbuf_used+message.length>recvbuf.length) { // 受信用のバッファがいっぱいのときは受信しない
        console.log("buffer full");
        return;
    }
    for(var i=0;i<message.length;i++) recvbuf[recvbuf_used+i]=message[i]; // 受信用のバッファにデータをコピーする
    recvbuf_used+=message.length; // 受信用バッファの使用量を増やす
});
socket.bind(23456,"0.0.0.0"); // UDPポート番号23456、任意アドレスからのデータグラムを受け入れる

const spk=new Speaker({ 
    channels: 1, // チャンネル数は1(モノラル)
    bitDepth: 16, // サンプリングデータのビット数は16 (デフォルトはリトルエンディアン)
    sampleRate: 48000, // サンプリングレート(Hz)
});

receiver.pipe(spk); 

