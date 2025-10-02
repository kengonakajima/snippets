#!/usr/bin/env node
'use strict';

const assert = require('assert');

const kSampleRate = 16000;
const kBlockSize = 64; // PAmac block size (4 ms)
const kFftSize = 128;
const kHalfFft = kFftSize >> 1;
const kHopSize = kFftSize; // no overlap; change to 64 for 50% overlap if needed
const kTickMs = Math.floor((kBlockSize / kSampleRate) * 1000);

let PortAudio = null;
if (process.platform === 'darwin') {
  PortAudio = require('./PAmac.node');
} else {
  console.error('This tool currently supports only macOS with PAmac.node.');
  process.exit(1);
}

PortAudio.initSampleBuffers(kSampleRate, kSampleRate, kBlockSize);
PortAudio.startMic();

const hannWindow = new Float32Array(kFftSize);
for (let i = 0; i < kFftSize; i++) {
  hannWindow[i] = 0.5 * (1 - Math.cos((2 * Math.PI * i) / (kFftSize - 1)));
}

const cosTable = new Float32Array(kHalfFft);
const sinTable = new Float32Array(kHalfFft);
for (let i = 0; i < kHalfFft; i++) {
  const angle = (-2 * Math.PI * i) / kFftSize;
  cosTable[i] = Math.cos(angle);
  sinTable[i] = Math.sin(angle);
}

const magnitudeHistory = new Float32Array(kHalfFft);
const smoothing = 0.6; // closer to 1 => slower decay

let sampleQueue = [];

function toInt16Array(x) {
  if (!x) return new Int16Array();
  if (ArrayBuffer.isView(x)) {
    if (x instanceof Int16Array) return x;
    return new Int16Array(x.buffer, x.byteOffset, Math.floor(x.byteLength / 2));
  }
  if (Buffer.isBuffer(x)) {
    return new Int16Array(x.buffer, x.byteOffset, Math.floor(x.byteLength / 2));
  }
  if (Array.isArray(x)) {
    const out = new Int16Array(x.length);
    for (let i = 0; i < x.length; i++) out[i] = x[i] | 0;
    return out;
  }
  throw new Error('Unsupported sample buffer type');
}

function reverseBits(x, bits) {
  let y = 0;
  for (let i = 0; i < bits; i++) {
    y = (y << 1) | (x & 1);
    x >>>= 1;
  }
  return y;
}

function fftRadix2(real, imag) {
  const n = real.length;
  assert((n & (n - 1)) === 0 && n !== 0);
  const levels = Math.log2(n) | 0;

  for (let i = 0; i < n; i++) {
    const j = reverseBits(i, levels);
    if (j > i) {
      const tmpR = real[i]; real[i] = real[j]; real[j] = tmpR;
      const tmpI = imag[i]; imag[i] = imag[j]; imag[j] = tmpI;
    }
  }

  for (let size = 2; size <= n; size <<= 1) {
    const half = size >> 1;
    const tableStep = n / size;
    for (let i = 0; i < n; i += size) {
      for (let j = 0; j < half; j++) {
        const angleIndex = j * tableStep;
        const cos = cosTable[angleIndex];
        const sin = sinTable[angleIndex];
        const k = i + j;
        const tReal = real[k + half] * cos - imag[k + half] * sin;
        const tImag = real[k + half] * sin + imag[k + half] * cos;
        real[k + half] = real[k] - tReal;
        imag[k + half] = imag[k] - tImag;
        real[k] += tReal;
        imag[k] += tImag;
      }
    }
  }
}

function samplesToMagnitudes(samples) {
  const windowed = new Float32Array(kFftSize);
  for (let i = 0; i < kFftSize; i++) {
    windowed[i] = samples[i] * hannWindow[i];
  }

  const real = new Float32Array(windowed);
  const imag = new Float32Array(kFftSize); // zero-initialized
  fftRadix2(real, imag);

  const mags = new Float32Array(kHalfFft);
  const scale = kFftSize * 0.5;
  for (let i = 0; i < kHalfFft; i++) {
    const mag = Math.sqrt(real[i] * real[i] + imag[i] * imag[i]) / scale;
    const smoothed = smoothing * magnitudeHistory[i] + (1 - smoothing) * mag;
    magnitudeHistory[i] = smoothed;
    mags[i] = smoothed;
  }
  return mags;
}

function magnitudeToChar(mag) {
  const db = 20 * Math.log10(mag + 1e-6);
  if (db < -55) return ' ';
  if (db < -40) return '.';
  if (db < -28) return ':';
  if (db < -16) return '*';
  return '@';
}

function emitSpectrumLine(magnitudes) {
  let line = '';
  for (let i = 0; i < magnitudes.length; i++) {
    line += magnitudeToChar(magnitudes[i]);
  }
  process.stdout.write(line + '\n');
}

function consumeQueue() {
  while (sampleQueue.length >= kFftSize) {
    const frame = sampleQueue.slice(0, kFftSize);
    sampleQueue.splice(0, kHopSize);
    const magnitudes = samplesToMagnitudes(frame);
    emitSpectrumLine(magnitudes);
  }
}

const timer = setInterval(() => {
  let rec = null;
  try {
    rec = PortAudio.getRecordedSamples();
  } catch (err) {
    console.error('Failed to fetch samples:', err);
    return;
  }
  const recArr = toInt16Array(rec);
  if (recArr.length > 0) {
    for (let i = 0; i < recArr.length; i++) {
      sampleQueue.push(recArr[i] / 32768);
    }
    if (typeof PortAudio.discardRecordedSamples === 'function') {
      PortAudio.discardRecordedSamples(recArr.length);
    }
  }
  consumeQueue();
}, Math.max(kTickMs, 1));

function shutdown() {
  clearInterval(timer);
  try { PortAudio && PortAudio.stopMic && PortAudio.stopMic(); } catch {}
}

process.on('SIGINT', () => {
  console.error('fftlog: stopped.');
  shutdown();
  process.exit(0);
});

process.on('SIGTERM', () => {
  shutdown();
  process.exit(0);
});

console.error('fftlog: 16kHz mic monitor (128-point FFT). Ctrl+Cで終了します。');
consumeQueue();
