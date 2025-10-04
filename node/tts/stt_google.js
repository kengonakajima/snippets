import dotenv from 'dotenv';
import fs from 'node:fs';
import path from 'node:path';
import { SpeechClient } from '@google-cloud/speech';
import record from 'node-record-lpcm16';

dotenv.config();

const SAMPLE_RATE = 16000;
const args = process.argv.slice(2);
const enableInterim = args.includes('--interim');
const credentials = process.env.GOOGLE_APPLICATION_CREDENTIALS?.trim();

if (!credentials) {
  console.error('GOOGLE_APPLICATION_CREDENTIALS が設定されていません。');
  process.exit(1);
}

const keyFile = path.isAbsolute(credentials)
  ? credentials
  : path.resolve(process.cwd(), credentials);

if (!fs.existsSync(keyFile)) {
  console.error(`サービスアカウント JSON が見つかりません: ${keyFile}`);
  process.exit(1);
}

const client = new SpeechClient({ keyFilename: keyFile });

const recognizeStream = client
  .streamingRecognize({
    config: {
      encoding: 'LINEAR16',
      sampleRateHertz: SAMPLE_RATE,
      languageCode: 'ja-JP',
    },
    interimResults: enableInterim,
  })
  .on('error', (error) => {
    console.error(`認識ストリームでエラーが発生しました: ${error.message}`);
    process.exitCode = 1;
  })
  .on('data', (data) => {
    const result = data.results?.[0];
    const transcript = result?.alternatives?.[0]?.transcript;
    if (!transcript) {
      return;
    }

    const trimmed = transcript.trim();
    if (!trimmed) {
      return;
    }

    const isFinal = result.isFinal === true;

    if (!enableInterim && !isFinal) {
      return;
    }

    const prefix = isFinal ? 'R:' : 'I:';
    console.log(`${prefix} ${trimmed}`);
  });

record
  .record({
    sampleRateHertz: SAMPLE_RATE,
    threshold: 0,
    silence: '1.0',
  })
  .stream()
  .on('error', (error) => {
    console.error(`録音中にエラーが発生しました: ${error.message}`);
    process.exit(1);
  })
  .pipe(recognizeStream);

if (enableInterim) {
  console.error('インタリム結果: ON');
}

console.error('録音を開始しました。終了する場合は Ctrl+C を押してください。');

process.on('SIGINT', () => {
  record.stop();
  recognizeStream.end();
  console.error('\n録音を終了しました。');
  process.exit(0);
});
