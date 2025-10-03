#!/usr/bin/env node
const { config } = require('dotenv');
const process = require('node:process');

config();

const API_KEY = process.env.PPLX_API_KEY;
if (!API_KEY) {
  console.error('PPLX_API_KEY が .env に設定されていません。');
  process.exit(1);
}

const query = process.argv.slice(2).join(' ').trim();
if (!query) {
  console.error('使い方: node sonar-pro-cli.js "質問内容"');
  process.exit(1);
}

const payload = {
  model: 'sonar-pro',
  messages: [
    {
      role: 'user',
      content: query,
    },
  ],
};

sendRequest(payload).catch((err) => {
  console.error('エラーが発生しました:', err && err.message ? err.message : err);
  if (err && err.data) {
    console.error(JSON.stringify(err.data, null, 2));
  }
  process.exit(1);
});

async function sendRequest(body) {
  if (typeof fetch !== 'function') {
    console.error('fetch が利用できない環境です。Node.js 18 以降を利用してください。');
    process.exit(1);
  }

  const response = await fetch('https://api.perplexity.ai/chat/completions', {
    method: 'POST',
    headers: {
      Authorization: `Bearer ${API_KEY}`,
      'Content-Type': 'application/json',
    },
    body: JSON.stringify(body),
  });

  const text = await response.text();
  let parsed = null;
  if (text) {
    try {
      parsed = JSON.parse(text);
    } catch (err) {
      parsed = text;
    }
  }

  if (!response.ok) {
    const error = new Error(`HTTP ${response.status}`);
    error.status = response.status;
    error.data = parsed;
    throw error;
  }

  console.log(typeof parsed === 'string' ? parsed : JSON.stringify(parsed, null, 2));
}
