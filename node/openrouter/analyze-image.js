#!/usr/bin/env node
require('dotenv').config();

const fs = require('fs/promises');
const path = require('path');
const axios = require('axios');

function showUsage() {
  console.error('使い方: node analyze-image.js <画像ファイルパス> <プロンプト文字列>');
  console.error('例: node analyze-image.js ./sample.jpg "画像の説明をしてください"');
  process.exit(1);
}

function resolveMimeType(ext) {
  if (!ext) {
    return null;
  }

  const lowerExt = ext.toLowerCase();

  if (lowerExt === '.jpg' || lowerExt === '.jpeg') {
    return 'image/jpeg';
  }

  if (lowerExt === '.png') {
    return 'image/png';
  }

  if (lowerExt === '.webp') {
    return 'image/webp';
  }

  if (lowerExt === '.gif') {
    return 'image/gif';
  }

  return null;
}

async function loadImageAsDataUrl(imagePath) {
  const absolutePath = path.resolve(process.cwd(), imagePath);

  const mimeType = resolveMimeType(path.extname(absolutePath));
  if (!mimeType) {
    console.error(`サポートされていない画像形式です: ${imagePath}`);
    process.exit(1);
  }

  const fileBuffer = await fs.readFile(absolutePath).catch((error) => {
    console.error(`画像ファイルを読み込めませんでした: ${error.message}`);
    process.exit(1);
  });

  return `data:${mimeType};base64,${fileBuffer.toString('base64')}`;
}

function extractContentText(content) {
  if (typeof content === 'string') {
    return content.trim();
  }

  if (Array.isArray(content)) {
    const joined = content
      .filter((part) => part && part.type === 'text' && typeof part.text === 'string')
      .map((part) => part.text.trim())
      .filter(Boolean)
      .join('\n');

    return joined.trim();
  }

  return '';
}

async function main() {
  const [, , imageArg, ...promptParts] = process.argv;

  if (!imageArg || promptParts.length === 0) {
    showUsage();
  }

  const prompt = promptParts.join(' ').trim();
  if (!prompt) {
    showUsage();
  }

  const apiKey = process.env.OPENROUTER_API_KEY;
  if (!apiKey) {
    console.error('OPENROUTER_API_KEY が .env に設定されていません。');
    process.exit(1);
  }

  const imageDataUrl = await loadImageAsDataUrl(imageArg);

  const requestBody = {
    model: 'openai/gpt-5-codex',
    messages: [
      {
        role: 'user',
        content: [
          { type: 'text', text: prompt },
          { type: 'image_url', image_url: { url: imageDataUrl } },
        ],
      },
    ],
  };

  const response = await axios
    .post('https://openrouter.ai/api/v1/chat/completions', requestBody, {
      headers: {
        Authorization: `Bearer ${apiKey}`,
        'Content-Type': 'application/json',
      },
    })
    .catch((error) => {
      if (error.response && error.response.data) {
        console.error(`APIエラー: ${JSON.stringify(error.response.data)}`);
      } else {
        console.error(`リクエストに失敗しました: ${error.message}`);
      }
      process.exit(1);
    });

  const firstChoice = response.data && response.data.choices && response.data.choices[0];
  if (!firstChoice || !firstChoice.message) {
    console.error('レスポンスに回答が含まれていません。');
    process.exit(1);
  }

  const text = extractContentText(firstChoice.message.content);
  if (!text) {
    console.log(JSON.stringify(firstChoice.message, null, 2));
    return;
  }

  console.log(text);
}

main();
