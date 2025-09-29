# LLM API Speed Benchmark Tool

このツールは、OpenAI互換APIを提供する各種LLMサービスのレスポンス速度と精度を測定するベンチマークツールです。

## 機能

- 10〜30個のランダムな1〜5桁の整数を生成
- LLMに奇数のみを選択させる
- 100回（カスタマイズ可能）のテストを実行
- 正答率と平均レスポンス時間を計測
- 詳細な統計情報（中央値、95パーセンタイル、最小/最大値）を出力

## セットアップ

1. 依存関係のインストール:
```bash
npm install
```

2. `.env`ファイルの作成:
```bash
cp .env.example .env
```

3. 使用するAPIキーを`.env`ファイルに設定

## 使い方

### 基本的な使用方法

```bash
node benchmark.js -m <model-name> -e <endpoint-url>
```

### オプション

- `-m, --model <model>` : 使用するモデル名（必須）
- `-e, --endpoint <endpoint>` : APIエンドポイントURL（必須）
- `-k, --key-env <env>` : APIキーの環境変数名（デフォルト: OPENAI_API_KEY）
- `-n, --iterations <number>` : テスト回数（デフォルト: 100）
- `-h, --help` : ヘルプを表示
- `-V, --version` : バージョンを表示

### 使用例

#### OpenAI GPT-3.5の場合:
```bash
node benchmark.js -m gpt-3.5-turbo -e https://api.openai.com/v1/chat/completions
```

#### OpenAI GPT-4の場合:
```bash
node benchmark.js -m gpt-4 -e https://api.openai.com/v1/chat/completions
```

#### OpenRouterの場合:
```bash
node benchmark.js -m meta-llama/llama-3.2-3b-instruct -e https://openrouter.ai/api/v1/chat/completions -k OPENROUTER_API_KEY
```

#### Groqの場合:
```bash
node benchmark.js -m llama3-8b-8192 -e https://api.groq.com/openai/v1/chat/completions -k GROQ_API_KEY
```

#### 少ないイテレーションでテスト:
```bash
node benchmark.js -m gpt-3.5-turbo -e https://api.openai.com/v1/chat/completions -n 10
```

## 出力例

```
============================================================
LLM API Speed Benchmark
============================================================
Model: gpt-3.5-turbo
Endpoint: https://api.openai.com/v1/chat/completions
Iterations: 100
============================================================
Running tests...

Progress: 100/100

============================================================
Results
============================================================
Total Iterations: 100
Successful Requests: 100/100 (100.00%)
Correct Responses: 98/100 (98.00%)
Average Response Time: 850ms
Median Response Time: 820ms
95th Percentile: 1250ms
Min Response Time: 650ms
Max Response Time: 1500ms
============================================================
```

## 対応サービス

OpenAI Completion API互換のエンドポイントに対応しています:

- OpenAI (GPT-3.5, GPT-4など)
- OpenRouter
- Groq
- Anthropic (互換エンドポイント経由)
- その他のOpenAI互換API

## 注意事項

- APIの利用料金が発生する場合があります
- レート制限を避けるため、各リクエスト間に100msの待機時間を設定しています
- タイムアウトは30秒に設定されています