require('dotenv').config();

const fs = require('node:fs');
const path = require('node:path');

const OPENROUTER_API_KEY = process.env.OPENROUTER_API_KEY;

if (!OPENROUTER_API_KEY) {
  console.error('環境変数 OPENROUTER_API_KEY が設定されていません。`.env` に `OPENROUTER_API_KEY=...` を追加して下さい。');
  process.exit(1);
}

const imagePath = path.join(__dirname, 'rana_standing.png');

if (!fs.existsSync(imagePath)) {
  console.error(`画像ファイルが見つかりません: ${imagePath}`);
  process.exit(1);
}

const imageBuffer = fs.readFileSync(imagePath);
const base64Image = imageBuffer.toString('base64');

const payload = {
  model: 'openai/gpt-5-codex',
  messages: [
    {
      role: 'user',
      content: [
        { type: 'text', text: 'この画像の内容を詳しく説明して下さい。' },
        {
          type: 'image_url',
          image_url: {
            url: `data:image/png;base64,${base64Image}`,
            detail: 'auto',
          },
        },
      ],
    },
  ],
};

// OpenRouter では Referer/X-Title を付けることが推奨されている
const headers = {
  Authorization: `Bearer ${OPENROUTER_API_KEY}`,
  'Content-Type': 'application/json',
};

if (process.env.OPENROUTER_HTTP_REFERER) {
  headers['HTTP-Referer'] = process.env.OPENROUTER_HTTP_REFERER;
}

if (process.env.OPENROUTER_APP_TITLE) {
  headers['X-Title'] = process.env.OPENROUTER_APP_TITLE;
}

async function main() {
  const response = await fetch('https://openrouter.ai/api/v1/chat/completions', {
    method: 'POST',
    headers,
    body: JSON.stringify(payload),
  });

  if (!response.ok) {
    const errorBody = await response.text();
    console.error(`APIリクエストに失敗しました: ${response.status} ${response.statusText}`);
    console.error(errorBody);
    process.exit(1);
  }

  const result = await response.json();
  const messageContent = result.choices?.[0]?.message?.content;
  const textOutput = Array.isArray(messageContent)
    ? messageContent
        .filter((part) => part.type === 'text')
        .map((part) => part.text)
        .join('\n')
    : messageContent;

  if (!textOutput) {
    console.warn('テキストの応答が取得できませんでした。レスポンス全体を表示します。');
    console.dir(result, { depth: null });
    return;
  }

  console.log('--- 画像解析結果 ---');
  console.log(textOutput);

  if (result.usage) {
    console.log('\n--- トークン使用量 ---');
    console.log(result.usage);
  }
}

main().catch((error) => {
  console.error('処理中にエラーが発生しました。');
  console.error(error);
  process.exit(1);
});
