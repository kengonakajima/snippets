#!/usr/bin/env node
const { config } = require('dotenv');
const { setTimeout: delay } = require('node:timers/promises');
const process = require('node:process');

config();

const API_KEY = process.env.PPLX_API_KEY;
if (!API_KEY) {
  console.error('PPLX_API_KEY が .env に設定されていません。');
  process.exit(1);
}

const query = process.argv.slice(2).join(' ').trim();
if (!query) {
  console.error('使い方: node sonar-deep-research-cli.js "質問内容"');
  process.exit(1);
}

if (typeof fetch !== 'function') {
  console.error('fetch が利用できない環境です。Node.js 18 以降を利用してください。');
  process.exit(1);
}

const POLL_INTERVAL_MS = 5000;

main().catch((err) => {
  console.error('エラーが発生しました:', err && err.message ? err.message : err);
  if (err && err.data) {
    console.error(JSON.stringify(err.data, null, 2));
  }
  process.exit(1);
});

async function main() {
  const job = await submitRequest(query);
  const jobId = job.id || job.request_id;
  if (!jobId) {
    console.error('ジョブIDを取得できませんでした。レスポンス:', JSON.stringify(job, null, 2));
    process.exit(1);
  }

  console.error(`ジョブを送信しました: ${jobId}`);

  while (true) {
    const payload = await fetchJob(jobId);
    const status = payload.status || payload.state || 'UNKNOWN';
    console.error(`status=${status}`);

    if (status === 'COMPLETED') {
      outputResult(payload);
      return;
    }

    if (status === 'FAILED' || status === 'ERROR') {
      const message = payload.error_message || payload.error?.message || payload.message || 'Sonar Deep Research が失敗を返しました。';
      console.error('ジョブが失敗しました:', message);
      process.exit(1);
    }

    await delay(POLL_INTERVAL_MS);
  }
}

async function submitRequest(content) {
  const response = await fetch('https://api.perplexity.ai/async/chat/completions', {
    method: 'POST',
    headers: {
      Authorization: `Bearer ${API_KEY}`,
      'Content-Type': 'application/json',
    },
    body: JSON.stringify({
      request: {
        model: 'sonar-deep-research',
        messages: [
          {
            role: 'user',
            content,
          },
        ],
      },
    }),
  });

  const text = await response.text();
  const parsed = parseMaybeJson(text);

  if (!response.ok) {
    const error = new Error(`HTTP ${response.status}`);
    error.status = response.status;
    error.data = parsed;
    throw error;
  }

  return parsed;
}

async function fetchJob(jobId) {
  const response = await fetch(`https://api.perplexity.ai/async/chat/completions/${jobId}`, {
    headers: {
      Authorization: `Bearer ${API_KEY}`,
    },
  });

  const text = await response.text();
  const parsed = parseMaybeJson(text);

  if (!response.ok) {
    const error = new Error(`HTTP ${response.status}`);
    error.status = response.status;
    error.data = parsed;
    throw error;
  }

  return parsed;
}

function parseMaybeJson(text) {
  if (!text) {
    return null;
  }
  try {
    return JSON.parse(text);
  } catch (err) {
    return text;
  }
}

function outputResult(payload) {
  const response = payload?.response;
  const content = response?.choices?.[0]?.message?.content;

  if (content) {
    console.log(content.trim());
    return;
  }

  console.log(JSON.stringify(payload, null, 2));
}
