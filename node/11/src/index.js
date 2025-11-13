#!/usr/bin/env node
const path = require('node:path');
const readline = require('node:readline');
const { spawnSync } = require('node:child_process');
require('dotenv').config({ path: path.resolve(process.cwd(), '.env') });

const { Scribe, AudioFormat, CommitStrategy, RealtimeEvents } = require('@elevenlabs/client');
const record = require('node-record-lpcm16');

const SAMPLE_RATE = Number(process.env.ELEVENLABS_SAMPLE_RATE) || 16000;
const MODEL_ID = process.env.ELEVENLABS_SCRIBE_MODEL_ID || 'scribe_v2_realtime';

const API_KEY = process.env.ELEVENLABS_API_KEY;

if (!API_KEY) {
  console.error('ELEVENLABS_API_KEY が .env から読み込めませんでした。');
  process.exit(1);
}

function ensureSoxExists() {
  const check = spawnSync('sox', ['--version'], { stdio: 'ignore' });
  if (check.error) {
    console.error('sox コマンドが見つかりません。Homebrew なら `brew install sox` で導入してください。');
    process.exit(1);
  }
}

const TOKEN_ENDPOINT = 'https://api.elevenlabs.io/v1/single-use-token/realtime_scribe';

async function requestSingleUseToken() {
  const response = await fetch(TOKEN_ENDPOINT, {
    method: 'POST',
    headers: {
      'xi-api-key': API_KEY,
      'Content-Type': 'application/json'
    }
  });

  if (!response.ok) {
    const body = await response.text();
    throw new Error(`Single-use token 取得に失敗しました (${TOKEN_ENDPOINT}): ${response.status} ${body}`);
  }

  const { token } = await response.json();
  if (!token) {
    throw new Error('レスポンスに token が含まれていません。');
  }
  return token;
}

function displayPartial(text) {
  readline.clearLine(process.stdout, 0);
  readline.cursorTo(process.stdout, 0);
  process.stdout.write(`[一時] ${text}`);
}

function displayFinal(text) {
  readline.clearLine(process.stdout, 0);
  readline.cursorTo(process.stdout, 0);
  console.log(`[確定] ${text}`);
}

function startRecording(connection) {
  const recorder = record.record({
    sampleRate: SAMPLE_RATE,
    threshold: 0,
    recorder: 'sox',
    audioType: 'raw',
    channels: 1,
    endOnSilence: false
  });

  recorder.stream().on('data', chunk => {
    connection.send({ audioBase64: chunk.toString('base64') });
  });

  return recorder;
}

function setupEventLogging(connection) {
  connection.on(RealtimeEvents.SESSION_STARTED, () => {
    console.log('セッションを開始しました。Ctrl+C で終了します。');
  });

  connection.on(RealtimeEvents.PARTIAL_TRANSCRIPT, message => {
    if (message?.text) {
      displayPartial(message.text);
    }
  });

  connection.on(RealtimeEvents.COMMITTED_TRANSCRIPT, message => {
    if (message?.text) {
      displayFinal(message.text);
    }
  });

  connection.on(RealtimeEvents.COMMITTED_TRANSCRIPT_WITH_TIMESTAMPS, message => {
    if (message?.text) {
      displayFinal(message.text);
    }
  });

  connection.on(RealtimeEvents.AUTH_ERROR, error => {
    console.error(`認証エラー: ${JSON.stringify(error)}`);
  });

  connection.on(RealtimeEvents.ERROR, error => {
    console.error(`接続エラー: ${JSON.stringify(error)}`);
  });
}

async function main() {
  ensureSoxExists();

  const token = await requestSingleUseToken();
  const connection = Scribe.connect({
    token,
    modelId: MODEL_ID,
    audioFormat: AudioFormat.PCM_16000,
    sampleRate: SAMPLE_RATE,
    commitStrategy: CommitStrategy.VAD
  });

  setupEventLogging(connection);
  const recorder = startRecording(connection);

  const cleanup = () => {
    recorder.stop();
    connection.close();
    process.exit(0);
  };

  process.on('SIGINT', () => {
    console.log('\n停止します...');
    cleanup();
  });
}

main().catch(error => {
  console.error(error);
  process.exit(1);
});
