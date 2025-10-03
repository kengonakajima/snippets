# Gemini 2.5 TTS サンプル

## 事前準備

1. Node.js 20 以降を利用してください。`nvm use 23.11.1` などで切り替え可能です。
2. 依存パッケージは `npm install` で取得します。
3. `.env.example` をコピーして `.env` を作成し、`GOOGLE_API_KEY` の値を設定できるようにしておきます。

## Google Gemini API キーの発行手順

1. ブラウザで [Google AI Studio の API Keys ページ](https://aistudio.google.com/apikey) にアクセスし、Google アカウントでログインします。
2. 必要に応じてプロジェクトを選択または作成し、「Create API key」をクリックします。
3. 表示された API キーを安全な場所にコピーします。キーを紛失した場合は再発行してください。
4. `.env` に以下のように記述して保存します。

   ```bash
   GOOGLE_API_KEY=取得したAPIキー
   ```

5. キーの漏洩を防ぐため、リポジトリにはコミットしないでください。必要に応じて Google AI Studio でキーをローテーションし、使わなくなったキーは削除します。

## APIキーの制限（推奨）

1. ブラウザで [Google Cloud Console の「API とサービス」→「認証情報」](https://console.cloud.google.com/apis/credentials) を開き、対象プロジェクトの API キーを選択します。
2. 「アプリケーションの制限」で利用形態に合わせた種別を選択します。
   - Web から呼び出す場合は `HTTP リファラ` に許可したいドメイン（例: `example.com/*`）を列挙します。
   - サーバー／CLI からのみ利用する場合は `IP アドレス` を選び、許可したい送信元 IP を登録します。
   - モバイルアプリで使うときは `Android アプリ` または `iOS アプリ` を選び、パッケージ名やバンドル ID を指定します。
3. 「API の制限」で「キーを制限」を選び、`Generative Language API (generativelanguage.googleapis.com)` のみに限定して保存します。
4. 制限後にテスト実行し、想定外のクライアントや API から拒否されることを確認してください。必要に応じて用途ごとにキーを分割し、短い有効範囲を維持します。

## 実行方法

```bash
node tts.js "こんにちは、世界"
```

- 合成音声は `generated.wav` に保存されます。
- 実行時間は標準出力に表示されます。

## トラブルシューティング

- `GOOGLE_API_KEY is not set` が表示された場合は `.env` の設定を確認してから再実行してください。
- `node -v` が `v20` 以上であることを確認してください。古いバージョンでは `@google/genai` のインストール時に警告や実行エラーが発生します。
