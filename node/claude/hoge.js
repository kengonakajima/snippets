




const fs = require('fs');
const Anthropic = require('@anthropic-ai/sdk');

// なぜ: コマンドライン引数から画像ファイルパスを取得し、ユーザーが解析対象の画像を指定できるようにするため
const imagePath = process.argv[2];
if (!imagePath) {
  console.error("使用法: node script.js <画像ファイルパス>");
  process.exit(1);
}

const imageData = fs.readFileSync(imagePath);
const base64Image = imageData.toString('base64');

// なぜ: ボタンの文言と画像を組み合わせ、公式仕様に沿った形式で画像情報を提供するため
const buttonText = "Submit"; // 特定したいボタンの表示テキスト
const userMessageContent = [
  { type: "text", text: `次の画像中に表示されている「${buttonText}」ボタンの座標を"x, y"形式で教えてください。` },
  { 
    type: "image", 
    image: { 
      source: {
        data: base64Image,      // なぜ: 画像のBase64エンコード済みデータを提供するため
        mime_type: "image/png"  // なぜ: 画像形式を明示して解析の正確性を向上させるため
      }
    }
  }
];
const messages = [{ role: "user", content: userMessageContent }];

(async () => {
  try {
    // なぜ: APIキーを環境変数から取得して、認証付きでAnthropic APIにアクセスするため
    const client = new Anthropic({ apiKey: process.env.ANTHROPIC_API_KEY });
    // なぜ: 指定したモデル (computer-use-2024-10-22) を利用して画像解析リクエストを送信するため
    const response = await client.messages.create({
      model: "computer-use-2024-10-22",
      messages: messages,
      max_tokens: 500
    });
    // なぜ: モデルからの応答から、ボタンの座標情報を取得するため
    const assistantReply = response.content;
    console.log("ボタンの座標:", assistantReply);
  } catch (error) {
    console.error("エラーが発生しました:", error);
  }
})();
