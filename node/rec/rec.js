const recorder = require('node-record-lpcm16'); // nodeモジュールを読み込む

recorder
    .record({
        sampleRate: 48000, // マイクデバイスのサンプリングレートを指定
        channels: 1,  // チャンネル数を指定(モノラル)              
        recordProgram: 'rec', // 録音用のバックエンドプログラム名を指定
    })
    .stream()
    .on('error', console.error) // エラーが起きたときにログを出力する
    .on('data', function(data) { // マイクからデータを受信する無名コールバック関数
        var vol = Math.abs(data.readInt16LE());  // 配列の先頭のサンプリングデータをひとつ読み込み、音量を得る
        var ntimes = vol / 512; // 音量が0~32767の値で得られるので512で割る(0~63)
        var bar = "*".repeat(ntimes); // アスタリスク文字を、音量に応じて0~63回繰り返す
        console.log("mic volume:", bar);
    });
console.log('Listening, press Ctrl+C to stop.');

