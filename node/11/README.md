# ElevenLabs Scribe 2 STT CLI

このリポジトリは、ElevenLabs の Scribe 2 ストリーミング API を使って Mac のマイク入力をリアルタイムに文字起こしする Node.js コンソールアプリです。

## 前提条件

- Node.js 18 以上（`fetch` API をネイティブ利用）
- SoX (`sox` コマンド)。Homebrew の場合は `brew install sox`
- ElevenLabs API Key（[ダッシュボード](https://elevenlabs.io)で取得）

## セットアップ

1. 依存ライブラリを導入します。
   ```bash
   npm install
   ```
2. `.env.example` を `.env` にコピーし、`ELEVENLABS_API_KEY` を設定します。
   ```bash
   cp .env.example .env
   # .env を編集して API キーを入力
   ```
3. (任意) `ELEVENLABS_SCRIBE_MODEL_ID` や `ELEVENLABS_SAMPLE_RATE` を変更したい場合は `.env` で上書きしてください。

## 実行

```bash
npm start
```

- プログラム起動後、単発トークンを取得して `scribe_v2_realtime` モデルへ接続し、マイク音声を VAD ベースで区切りながら送信します。
- 部分文字起こしは `[一時]`、確定結果は `[確定]` としてコンソールに表示されます。
- `Ctrl + C` で終了できます。

## トラブルシューティング

- `sox コマンドが見つかりません` と出る場合は ```brew install sox``` を実行してください。
- API Key の権限不足や入力ミスがあると単発トークン取得でエラーになります。API Key を再確認してください。

## 仕組み概要

1. `.env` から API Key と各種設定を読み込みます。
2. ElevenLabs のシングルユーストークン API に `POST` し、Scribe WebSocket 用トークンを取得します。
3. `@elevenlabs/client` の `Scribe.connect` でリアルタイム接続を張り、`node-record-lpcm16` で得た 16kHz PCM を Base64 へ変換して送信します。
4. 受信した Partial / Committed transcript をコンソールに描画します。

## 注意

- ソース内で try/catch を多用していないため、エラー発生時はプロセスが終了します。問題点を把握しやすくするための方針です。
- ElevenLabs のモデル ID や API の仕様が変更された場合は、公式ドキュメントを参照してエンドポイントを更新してください。
