// transcribe-file.js
import "dotenv/config";
import fs from "node:fs";
import OpenAI from "openai";

const client = new OpenAI({ apiKey: process.env.OPENAI_API_KEY });

async function main() {
  // 任意の音声ファイル（wav, mp3, m4a など）
  const file = fs.createReadStream("./japanese_sample_24k.wav");
  const resp = await client.audio.transcriptions.create({
    file,
    model: "gpt-4o-transcribe",        // ★ ここがポイント
    // language: "ja",                  // 日本語固定したいなら指定（任意）
    // temperature: 0,                  // 必要に応じて
    // response_format: "json"          // 既定でOK
  });
  console.log(resp.text);               // 文字起こし結果
}
main();
