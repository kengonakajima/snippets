import dotenv from 'dotenv';
import wav from 'wav';
import { performance } from 'node:perf_hooks';
import https from 'node:https';

dotenv.config();

const text = process.argv.slice(2).join(' ').trim();
const SAMPLE_RATE_HZ = 22050;
const SPEAKING_RATE = 1.4;

if (!text) {
  console.error('Usage: node tts_ttsapi.js "<text>"');
  process.exit(1);
}

if (!process.env.GOOGLE_TTS_API_KEY) {
  console.error('GOOGLE_TTS_API_KEY is not set in the environment.');
  process.exit(1);
}

function saveWav(filename, pcm, { channels = 1, rate = SAMPLE_RATE_HZ, sampleWidth = 2 } = {}) {
  return new Promise((resolve, reject) => {
    const writer = new wav.FileWriter(filename, {
      channels,
      sampleRate: rate,
      bitDepth: sampleWidth * 8,
    });

    writer.on('finish', resolve);
    writer.on('error', reject);

    writer.write(pcm);
    writer.end();
  });
}

function postTextToSpeech(apiKey, payload) {
  return new Promise((resolve, reject) => {
    const requestBody = JSON.stringify(payload);

    const req = https.request(
      {
        hostname: 'texttospeech.googleapis.com',
        path: `/v1/text:synthesize?key=${encodeURIComponent(apiKey)}`,
        method: 'POST',
        headers: {
          'Content-Type': 'application/json; charset=utf-8',
          'Content-Length': Buffer.byteLength(requestBody),
        },
      },
      (res) => {
        let data = '';
        res.setEncoding('utf8');

        res.on('data', (chunk) => {
          data += chunk;
        });

        res.on('end', () => {
          if (res.statusCode && res.statusCode >= 200 && res.statusCode < 300) {
            resolve(JSON.parse(data));
            return;
          }

          reject(new Error(`Text-to-Speech API error ${res.statusCode}: ${data}`));
        });
      }
    );

    req.on('error', reject);
    req.write(requestBody);
    req.end();
  });
}

async function synthesize(textInput, apiKey) {
  const payload = {
    input: { text: textInput },
    voice: {
      languageCode: 'ja-JP',
      name: 'ja-JP-Standard-A',
    },
    audioConfig: {
      audioEncoding: 'LINEAR16',
      sampleRateHertz: SAMPLE_RATE_HZ,
      speakingRate: SPEAKING_RATE,
    },
  };

  const response = await postTextToSpeech(apiKey, payload);
  const audioContent = response?.audioContent;

  if (!audioContent) {
    throw new Error('No audioContent returned from Text-to-Speech API.');
  }

  return Buffer.from(audioContent, 'base64');
}

async function main() {
  const start = performance.now();
  const pcmBuffer = await synthesize(text, process.env.GOOGLE_TTS_API_KEY);
  await saveWav('generated.wav', pcmBuffer);
  const end = performance.now();
  console.log('Audio generated: generated.wav');
  console.log(`Elapsed time: ${((end - start) / 1000).toFixed(2)}s`);
}

main();
