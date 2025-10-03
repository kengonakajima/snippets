# Gemini 2.5 TTS サンプル

## 事前準備

1. Node.js 20 以降を利用してください。`nvm use 23.11.1` などで切り替え可能です。
2. 依存パッケージは `npm install` で取得します。
3. `.env.example` をコピーして `.env` を作成し、`GOOGLE_API_KEY` と `GOOGLE_TTS_API_KEY` を設定できるようにしておきます。

## Google Gemini API キーの発行手順

1. ブラウザで [Google AI Studio の API Keys ページ](https://aistudio.google.com/apikey) にアクセスし、Google アカウントでログインします。
2. 必要に応じてプロジェクトを選択または作成し、「Create API key」をクリックします。
3. 表示された API キーを安全な場所にコピーします。キーを紛失した場合は再発行してください。
4. `.env` に以下のように記述して保存します。

   ```bash
   GOOGLE_API_KEY=取得したAPIキー
   ```

5. キーの漏洩を防ぐため、リポジトリにはコミットしないでください。必要に応じて Google AI Studio でキーをローテーションし、使わなくなったキーは削除します。

## Google Cloud Text-to-Speech API キーの発行手順

1. [Google Cloud Console](https://console.cloud.google.com/) にログインし、右上のプロジェクトセレクタからターゲットプロジェクトを選択または作成します。
2. 左側メニューの「API とサービス」→「ライブラリ」を開き、`Text-to-Speech API` を検索して有効化します。新規プロジェクトの場合は課金アカウントのリンクも求められます。
3. 「API とサービス」→「認証情報」に移動し、「認証情報を作成」→「API キー」を選択してキーを生成します。
4. 生成されたキーを `.env` の `GOOGLE_TTS_API_KEY` に設定します。
5. 不要な誤用を避けるため直後にキー制限を設定します（下記「APIキーの制限」参照）。

## APIキーの制限（推奨）

1. ブラウザで [Google Cloud Console の「API とサービス」→「認証情報」](https://console.cloud.google.com/apis/credentials) を開き、対象プロジェクトの API キーを選択します。
2. 「アプリケーションの制限」で利用形態に合わせた種別を選択します。
   - Web から呼び出す場合は `HTTP リファラ` に許可したいドメイン（例: `example.com/*`）を列挙します。
   - サーバー／CLI からのみ利用する場合は `IP アドレス` を選び、許可したい送信元 IP を登録します。
   - モバイルアプリで使うときは `Android アプリ` または `iOS アプリ` を選び、パッケージ名やバンドル ID を指定します。
3. 「API の制限」で「キーを制限」を選び、用途に応じて `Generative Language API (generativelanguage.googleapis.com)` や `Cloud Text-to-Speech API (texttospeech.googleapis.com)` のみに限定して保存します。
4. 制限後にテスト実行し、想定外のクライアントや API から拒否されることを確認してください。必要に応じて用途ごとにキーを分割し、短い有効範囲を維持します。

## 実行方法

### Gemini 2.5 TTS サンプル

```bash
node tts_gemini.js "こんにちは、世界"
```

- 合成音声は `generated.wav` に保存されます。
- 実行時間は標準出力に表示されます。
- 音声は `SPEAKING_RATE = 1.8` に設定されており標準より速いスピードで再生されます。必要に応じてファイル先頭の定数を調整してください。citeturn1search0

### Cloud Text-to-Speech API サンプル

```bash
node tts_ttsapi.js "こんにちは、世界"
```

- こちらも `generated.wav` に上書き保存します。両方の結果を保持したい場合は実行前にファイルをリネームしてください。
- `tts_ttsapi.js` でも `SPEAKING_RATE = 1.8` を指定して高速再生します。値を大きくすると話速が上がり、小さくすると下がります。citeturn2search0

## トラブルシューティング

- `GOOGLE_API_KEY is not set` が表示された場合は `.env` の設定を確認してから再実行してください。
- `GOOGLE_TTS_API_KEY is not set` が表示された場合は Text-to-Speech API 用のキーを設定してください。
- `node -v` が `v20` 以上であることを確認してください。古いバージョンでは `@google/genai` のインストール時に警告や実行エラーが発生します。
