const http = require('http');
const fs = require('fs');

const BASE_OPTIONS = {
  hostname: 'localhost',
  port: 50021,
};

function httpRequest(path, { method = 'GET', headers = {}, body = null } = {}) {
  return new Promise((resolve, reject) => {
    const options = { ...BASE_OPTIONS, path, method, headers };
    const req = http.request(options, (res) => {
      const chunks = [];
      res.on('data', (chunk) => chunks.push(chunk));
      res.on('end', () => {
        const buffer = Buffer.concat(chunks);
        if (res.statusCode && res.statusCode >= 200 && res.statusCode < 300) {
          resolve({ buffer, headers: res.headers });
          return;
        }
        const message = buffer.length ? buffer.toString('utf8') : res.statusMessage || '';
        const error = new Error(`HTTP ${res.statusCode} ${res.statusMessage || ''} ${message}`.trim());
        reject(error);
      });
    });

    req.on('error', reject);

    if (body) {
      req.write(body);
    }

    req.end();
  });
}

async function fetchSpeakers() {
  const { buffer } = await httpRequest('/speakers');
  return JSON.parse(buffer.toString('utf8'));
}

function selectZundamonStyleId(speakers) {
  for (const speaker of speakers) {
    if (typeof speaker.name === 'string' && speaker.name.includes('ずんだもん')) {
      if (Array.isArray(speaker.styles)) {
        const normalStyle = speaker.styles.find((style) => style.name === 'ノーマル');
        const picked = normalStyle || speaker.styles[0];
        return picked ? picked.id : null;
      }
      return null;
    }
  }
  return null;
}

async function createAudioQuery(text, speakerId) {
  const path = `/audio_query?text=${encodeURIComponent(text)}&speaker=${speakerId}`;
  const { buffer } = await httpRequest(path, {
    method: 'POST',
    headers: {
      'Content-Type': 'application/json',
    },
  });
  return JSON.parse(buffer.toString('utf8'));
}

async function synthesizeWav(audioQuery, speakerId) {
  const body = JSON.stringify(audioQuery);
  const { buffer } = await httpRequest(`/synthesis?speaker=${speakerId}`, {
    method: 'POST',
    headers: {
      'Content-Type': 'application/json',
      'Content-Length': Buffer.byteLength(body),
    },
    body,
  });
  return buffer;
}

async function main() {
  const [, , ...args] = process.argv;
  if (args.length === 0) {
    console.error('使い方: node tts.js "読み上げたいテキスト"');
    process.exit(1);
  }
  const text = args.join(' ');

  const speakers = await fetchSpeakers();
  const styleId = selectZundamonStyleId(speakers);

  if (typeof styleId !== 'number') {
    console.error('ずんだもんのスタイルIDが見つかりませんでした。VOICEVOXが起動しているか確認してください。');
    process.exit(1);
  }

  const startedAt = Date.now();
  const audioQuery = await createAudioQuery(text, styleId);
  audioQuery.speedScale = 1.3;
  const wavBuffer = await synthesizeWav(audioQuery, styleId);
  const finishedAt = Date.now();

  fs.writeFileSync('generated.wav', wavBuffer);
  const elapsedSec = ((finishedAt - startedAt) / 1000).toFixed(3);
  console.log(`generated.wav を作成しました (所要時間: ${elapsedSec} 秒)`);
}

main().catch((error) => {
  console.error('エラーが発生しました:', error.message);
  process.exit(1);
});
