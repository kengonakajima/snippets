import dotenv from 'dotenv';
import { GoogleGenAI } from '@google/genai';
import wav from 'wav';
import { performance } from 'node:perf_hooks';

dotenv.config();

const text = process.argv.slice(2).join(' ').trim();
const SPEAKING_RATE = 1.8;

if (!text) {
  console.error('Usage: node tts_gemini.js "<text>"');
  process.exit(1);
}

if (!process.env.GOOGLE_API_KEY) {
  console.error('GOOGLE_API_KEY is not set in the environment.');
  process.exit(1);
}

function saveWav(filename, pcm, { channels = 1, rate = 24000, sampleWidth = 2 } = {}) {
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

async function generateSpeech(ai, inputText) {
  const response = await ai.models.generateContent({
    model: 'gemini-2.5-flash-preview-tts',
    contents: [{ parts: [{ text: inputText }] }],
    config: {
      responseModalities: ['AUDIO'],
      speechConfig: {
        voiceConfig: {
          prebuiltVoiceConfig: {
            voiceName: 'Kore',
            speakingRate: SPEAKING_RATE,
          },
        },
      },
    },
  });

  const audioData = response.candidates?.[0]?.content?.parts?.[0]?.inlineData?.data;

  if (!audioData) {
    throw new Error('No audio data returned from the API.');
  }

  return Buffer.from(audioData, 'base64');
}

async function main() {
  const ai = new GoogleGenAI({ apiKey: process.env.GOOGLE_API_KEY });
  const start = performance.now();
  const pcmBuffer = await generateSpeech(ai, text);
  await saveWav('generated.wav', pcmBuffer);
  const end = performance.now();
  console.log('Audio generated: generated.wav');
  console.log(`Elapsed time: ${((end - start) / 1000).toFixed(2)}s`);
}

main();
