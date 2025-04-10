const opus = require('@discordjs/opus');
const fs = require('fs');

// Opusデコーダーを初期化
console.log("opus:",opus);

const decoder = new opus.OpusEncoder(48000, 1); // ここでの48000はサンプリングレート、2はチャンネル数

// Opusエンコードされたデータを読み込む
const encodedData = fs.readFileSync('./wasm_only_file.opus');

// デコードする
const decoded = decoder.decode(encodedData);

// デコードされたデータを扱う
// 例：ファイルとして保存する
fs.writeFileSync('decoded.raw', decoded);

