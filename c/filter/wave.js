const fs = require('fs');
const { createCanvas } = require('canvas');

// 入力ファイルと出力ファイルのパス
const inputFilePath = process.argv[2];
const outputFilePath = process.argv[3];

// 波形のサイズとスタイル
const width = 800;
const height = 400;
const backgroundColor = 'white';
const waveColor = 'black';
const strokeWidth = 1;

// 入力ファイルを読み込む
const rawData = fs.readFileSync(inputFilePath);

// キャンバスを作成
const canvas = createCanvas(width, height);
const ctx = canvas.getContext('2d');

// 背景を塗りつぶす
ctx.fillStyle = backgroundColor;
ctx.fillRect(0, 0, width, height);

// 波形を描画
ctx.strokeStyle = waveColor;
ctx.lineWidth = strokeWidth;
ctx.beginPath();
ctx.moveTo(0, height / 2);

const numSamples = rawData.length / 2;
const dx = width / numSamples;

for (let i = 0; i < numSamples; i++) {
  const sample = rawData.readInt16LE(i * 2);
  const x = i * dx;
  const y = (1 - sample / 32768) * height / 2;
  ctx.lineTo(x, y);
}

ctx.stroke();

// PNGファイルを保存
const buffer = canvas.toBuffer('image/png');
fs.writeFileSync(outputFilePath, buffer);

console.log('波形がPNGファイルに保存されました。');
