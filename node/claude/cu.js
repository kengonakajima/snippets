const fs = require('fs');
const https = require('https');
const sharp = require('sharp');
const path = require('path');

// APIキーを環境変数から取得
const apiKey = process.env.ANTHROPIC_API_KEY;
if (!apiKey) {
  console.error('Error: ANTHROPIC_API_KEY environment variable is not set');
  process.exit(1);
}

async function drawButtonsOnImage(imagePath, buttons) {
  const originalImage = sharp(imagePath);
  const metadata = await originalImage.metadata();
  
  // SVGで矩形と番号を描画
  const svgBuffer = Buffer.from(`
    <svg width="${metadata.width}" height="${metadata.height}">
      ${buttons.map((button, index) => `
        <rect
          x="${button.left}"
          y="${button.top}"
          width="${button.right - button.left}"
          height="${button.bottom - button.top}"
          fill="none"
          stroke="blue"
          stroke-width="2"
        />
        <text
          x="${button.left + (button.right - button.left) / 2}"
          y="${button.top + (button.bottom - button.top) / 2}"
          font-family="Arial"
          font-size="24"
          fill="blue"
          text-anchor="middle"
          dominant-baseline="middle"
        >${index + 1}</text>
      `).join('')}
    </svg>
  `);

  // 元の画像の上にSVGを重ねる
  const outputPath = imagePath.replace(/\.[^/.]+$/, '') + '.generated.png';
  await originalImage
    .composite([{
      input: svgBuffer,
      top: 0,
      left: 0
    }])
    .toFile(outputPath);
    
  return outputPath;
}

async function analyzeImage(imagePath) {
  // 画像ファイルの存在確認
  if (!fs.existsSync(imagePath)) {
    console.error(`Error: Image file not found: ${imagePath}`);
    process.exit(1);
  }

  // 画像のメタデータを取得
  const metadata = await sharp(imagePath).metadata();
  console.log('画像サイズ:', { width: metadata.width, height: metadata.height });

  // 画像をBase64エンコード
  const imageBuffer = fs.readFileSync(imagePath);
  const base64Image = imageBuffer.toString('base64');

  const requestData = {
    model: "claude-3-5-sonnet-20241022",
    max_tokens: 1024,
    stream: true,  // ストリーミングを有効化
    tools: [
      {
        type: "computer_20241022",
        name: "computer",
        display_width_px: metadata.width,
        display_height_px: metadata.height,
        display_number: 1
      },
      {
        type: "text_editor_20241022",
        name: "str_replace_editor"
      },
      {
        type: "bash_20241022",
        name: "bash"
      }
    ],
    messages: [{
      role: "user",
      content: [
        {
          type: "image",
          source: {
            type: "base64",
            media_type: "image/png",
            data: base64Image,
          }
        },
        {
          type: "text",
          text: "この画像内から以下の4つの要素の位置情報を抽出してください。説明は不要で、JSONデータのみを返してください：\n\n1. 「作成状況」タブ（オレンジ色の背景を持つタブ領域全体）\n2. 「複写」ボタン（テキストを含む操作可能な領域全体）\n3. 「愛知育郎」という名前の選択ボタン（名前を含む操作可能な領域全体）\n4. 「国保請求しない」のチェックボックス（チェックマークが表示される四角い領域のみ。テキストラベルは含まない）\n\n形式: [{\"left\": 数値, \"top\": 数値, \"right\": 数値, \"bottom\": 数値, \"text\": \"要素の説明\"}]\n\n注意：\n- 指定した4つの要素の情報のみを返してください\n- タブやボタンは操作可能な領域全体の座標を返してください\n- チェックボックスはチェックマークが表示される四角い領域のみの座標を返してください\n- 説明文は一切書かないでください\n- JSONデータのみを出力してください"
        }
      ]
    }]
  };

  const options = {
    hostname: 'api.anthropic.com',
    path: '/v1/messages',
    method: 'POST',
    headers: {
      'Content-Type': 'application/json',
      'x-api-key': apiKey,
      'anthropic-version': '2023-06-01',
      'anthropic-beta': 'computer-use-2024-10-22'
    }
  };

  return new Promise((resolve, reject) => {
    console.log('\n送信するリクエスト:');
    console.log(JSON.stringify(requestData, null, 2));

    const req = https.request(options, (res) => {
      let buffer = '';
      let completeResponse = '';

      res.on('data', (chunk) => {
        buffer += chunk;
        console.log("CHUNK:",chunk.toString());
        
        const lines = buffer.split('\n');
        buffer = lines.pop();

        for (const line of lines) {
          if (line.trim() === '') continue;
          
          if (line.startsWith('event:')) {
            continue;
          }
          
          const jsonStr = line.replace(/^data: /, '');
          
          if (jsonStr === '[DONE]') {
            console.log('\n[完了]');
            continue;
          }

          try {
            const data = JSON.parse(jsonStr);
            if (data.delta?.text) {
              completeResponse += data.delta.text;
            }
          } catch (e) {
          }
        }
      });

      res.on('end', async () => {
        try {
          const jsonMatch = completeResponse.match(/\[\s*({[^}]+}(?:\s*,\s*{[^}]+})*)/);
          if (jsonMatch) {
            let buttons;
            try {
              let jsonStr = jsonMatch[0];
              if (!jsonStr.endsWith(']')) {
                jsonStr += ']';
              }
              
              jsonStr = jsonStr.replace(/,\s*{[^}]*$/g, ']');
              
              buttons = JSON.parse(jsonStr);
              
              if (buttons && buttons.length > 0) {
                console.log('\n検出されたボタン一覧:');
                buttons.forEach((btn, index) => {
                  console.log(`${index + 1}. "${btn.text}" (位置: ${btn.left},${btn.top} -> ${btn.right},${btn.bottom})`);
                });
                console.log(`\n合計 ${buttons.length} 個のボタンを検出しました。\n`);

                const outputPath = await drawButtonsOnImage(imagePath, buttons);
                console.log(`\n生成された画像を保存しました: ${outputPath}`);
                
                resolve(buttons);
              } else {
                throw new Error('ボタンデータが空です');
              }
            } catch (parseError) {
              console.error('JSONパースエラー:', parseError);
              console.error('マッチしたテキスト:', jsonMatch[0]);
              reject(parseError);
            }
          } else {
            console.error('レスポンスからJSONデータが見つかりませんでした');
            console.error('レスポンス全体の長さ:', completeResponse.length);
            console.error('レスポンスの最初の500文字:', completeResponse.substring(0, 500));
            throw new Error('レスポンスからJSONデータが見つかりませんでした');
          }
        } catch (e) {
          console.error('レスポンス処理エラー:', e);
          reject(e);
        }
      });

      res.on('error', (error) => {
        console.error('Response Error:', error);
        reject(error);
      });
    });

    req.on('error', (error) => {
      console.error('Request Error:', error);
      reject(error);
    });

    req.write(JSON.stringify(requestData));
    req.end();
  });
}

// コマンドライン引数から画像パスを取得
if (require.main === module) {
  const imagePath = process.argv[2];
  const jsonPath = process.argv[3];  // JSONファイルのパスを追加

  if (!imagePath) {
    console.error('Usage: node analyze_image.js <path-to-image> [path-to-json]');
    console.error('Make sure to set the ANTHROPIC_API_KEY environment variable');
    process.exit(1);
  }

  // JSONファイルが指定された場合は、そのデータを使用して直接描画
  if (jsonPath) {
    try {
      const jsonData = JSON.parse(fs.readFileSync(jsonPath, 'utf8'));
      drawButtonsOnImage(imagePath, jsonData)
        .then(outputPath => {
          console.log(`\n生成された画像を保存しました: ${outputPath}`);
        })
        .catch(error => {
          console.error('Error:', error);
          process.exit(1);
        });
    } catch (error) {
      console.error('JSONファイルの読み込みエラー:', error);
      process.exit(1);
    }
  } else {
    // JSONファイルが指定されていない場合は、APIを使用して解析
    analyzeImage(imagePath).catch(error => {
      console.error('Error:', error);
      process.exit(1);
    });
  }
}

module.exports = { analyzeImage, drawButtonsOnImage };  // drawButtonsOnImageをエクスポートに追加
