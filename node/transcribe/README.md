# 音声文字起こしスクリプトについて

## 概要
- `transcribe-file.js`: OpenAI Node SDK の `audio.transcriptions.create`（モデル `gpt-4o-transcribe`）を使ってローカル音声を一括変換します。`dotenv` 経由で `OPENAI_API_KEY` を読み込み、返却された `text` を標準出力へ表示します。
- `transcribe-realtime.js`: WAV ファイルをリアルタイム API（WebSocket `gpt-4o-realtime-preview`）へストリーミング送信し、`conversation.item.input_audio_transcription.*` イベントをすべて集約して最終的な全文を得ます。
- `transcribe-pa.js`: PAmac.node (PortAudio) を使って Mac のマイク入力をリアルタイム API へ送信し、逐次的に文字起こしします。

## 必要環境
- Node.js v23.11.1 以上（OpenAI SDK がネイティブ `fetch` を使用）
- npm 依存: `openai`, `dotenv`, `ws`
- システムツール: `ffmpeg`, `ffprobe`（音声変換と長さ計測に使用）
- `transcribe-pa.js` 利用時は PAmac.node（PortAudio のネイティブバインディング）が `./PAmac.node` に必要
- `.env` ファイル: 少なくとも `OPENAI_API_KEY` を定義（後述のオプションもここで設定可能）

## transcribe-file.js の動作
- 使用 API: `client.audio.transcriptions.create({ file, model: "gpt-4o-transcribe" })`
- デフォルト音源: `japanese_sample_24k.wav`（スクリプトのパスを変更するか、呼び出し側で別ファイルを指定してください）
- ストリーミング不要・簡易バッチ処理が欲しい場合に適しています。
- 実行例:
  ```bash
  set -a && source .env && /Users/ringo/.nvm/versions/node/v23.11.1/bin/node transcribe-file.js
  ```

## transcribe-realtime.js の動作
- 使用 API: `wss://api.openai.com/v1/realtime?model=gpt-4o-realtime-preview`
  - ヘッダー `Authorization: Bearer <OPENAI_API_KEY>` と `OpenAI-Beta: realtime=v1` を付与
- セッション設定:
  - `input_audio_transcription` に `gpt-4o-transcribe` を指定
  - 自動レスポンスは無効化（`create_response: false`, `interrupt_response: false`）
  - 温度は `REALTIME_TEMPERATURE`（既定 0.6）
- 音声送信:
  - `ffmpeg` で 16 kHz モノラル PCM16 へ変換し、チャンクをリアルタイム相当の速度で WebSocket 送信
  - `ffprobe` で音源の長さを測定し、タイムアウトを「音源長 + 10 秒」に設定（取得できない場合は `REALTIME_TIMEOUT_MS` の既定 10000ms を使用）
  - キューが一定量を超えたときは `ffmpeg` を一時停止し、送信に追いついたら再開
- 文字起こし集約:
  - VAD が作成する各 `conversation.item` を追跡し、デルタと完了イベントを蓄積
  - すべてのアイテムが `conversation.item.input_audio_transcription.completed` を受け取った時点でまとめてログ出力し、セッションを終了
- 調整可能な環境変数:
  - `REALTIME_TIMEOUT_MS`: 音源長が取得できない場合のフォールバック値
  - `REALTIME_GRACE_MS`: 音源長に追加する猶予時間（既定 10000ms）
  - `REALTIME_TEMPERATURE`: モデル温度
- `REALTIME_MAX_QUEUE`: 送信前にバッファするチャンク数（既定 50）
- 実行例（デフォルトは `japanese_sample_24k.wav`。別ファイルは引数で指定）：
  ```bash
  set -a && source .env && /Users/ringo/.nvm/versions/node/v23.11.1/bin/node transcribe-realtime.js
  ```

### transcribe-pa.js の補足
- 使用 API / 設定は `transcribe-realtime.js` と同様だが、音声ソースがマイク入力。
- 取得した 16kHz PCM16 を 100ms ごと（`PA_CHUNK_MS` 環境変数で調整可）に WebSocket へ送信し、約 2 秒ごと（`PA_COMMIT_INTERVAL_MS`）に `input_audio_buffer.commit` を発行。
- VAD が区切った各 `conversation.item` の最終テキストを `> ...` 形式で標準出力に逐次表示。
- 進捗情報やエラーログは `stderr` に JSON 形式で出力し、標準出力は認識された文字のみを流す。
- 調整可能な環境変数（上記に加えて）
  - `PA_SAMPLE_RATE` / `PA_BLOCK_SIZE`: PAmac の初期化パラメータ（既定 16000 / 64）
  - `PA_POLL_INTERVAL_MS`: マイクバッファの読み取り間隔（既定 20）
  - `PA_RESPONSE_MODALITIES`: `response.create` に指定するモダリティ配列（デフォルトは `text` のみ）
  - `PA_SESSION_TIMEOUT_MS`: セッション無通信タイムアウト（既定 300000ms）
  - `PA_TOTAL_DURATION_MS`: プログラム全体の最長稼働時間。0 で無効（既定 0）
- 実行例:
  ```bash
  set -a && source .env && /Users/ringo/.nvm/versions/node/v23.11.1/bin/node transcribe-pa.js
  ```

## 注意点 / トラブルシューティング
- `OPENAI_API_KEY` が Realtime API を使用できる権限を持っていることを確認してください。
- `ffprobe` が未導入の場合はインストールしてください（通常は `ffmpeg` と同時に導入）。失敗するとログに `ffprobe_failed` が出てフォールバック値へ切替わります。
- 制限の厳しいシェル環境では長時間の音声がタイムアウトすることがあります。別環境で実行するか、クリップを短くして対応してください。
- `.env` は秘密情報を含むため、リポジトリへコミットしないでください。
