# Vector Search with OpenAI and Redis

OpenAI Embeddings APIとRedis Vector Setsを使用したベクトル検索のデモプロジェクトです。

## 機能

- OpenAI Embeddings APIを使用したテキストのベクトル化
- Redis Vector Setsへのベクトルデータ保存
- コサイン類似度を使用した類似文書検索
- カテゴリフィルタリング機能
- インタラクティブな検索インターフェース

## 必要要件

- Node.js 18以上
- Redis Stack Server (Vector Search機能付き)
- OpenAI APIキー

## セットアップ

### 1. Redis Stackのインストール

Docker を使用する場合:
```bash
docker run -d --name redis-stack -p 6379:6379 -p 8001:8001 redis/redis-stack:latest
```

macOS (Homebrew):
```bash
brew tap redis-stack/redis-stack
brew install redis-stack
redis-stack-server
```

### 2. プロジェクトのセットアップ

```bash
# 依存関係のインストール
npm install

# 環境変数の設定
cp .env.example .env
# .envファイルを編集してOpenAI APIキーを設定
```

### 3. データベースの初期化

```bash
# サンプルデータをダウンロードしてRedisに保存
npm run setup
```

## 使用方法

### デモの実行
```bash
npm start
```

### インタラクティブ検索
```bash
npm run search
```

## プロジェクト構成

- `index.js` - メインデモファイル
- `setup.js` - データベース初期化スクリプト
- `search.js` - インタラクティブ検索インターフェース
- `fetchData.js` - サンプルデータの取得
- `embeddings.js` - OpenAI Embeddings API連携
- `redisClient.js` - Redis Vector Sets操作

## サンプルデータ

30個のサンプル文書が含まれています：
- ワイン説明文 (10個)
- ニュース記事 (10個)
- 製品説明文 (10個)

## 環境変数

`.env`ファイルで以下の設定が可能：

```
OPENAI_API_KEY=your_api_key_here
REDIS_HOST=localhost
REDIS_PORT=6379
REDIS_PASSWORD=
REDIS_DB=0
VECTOR_INDEX_NAME=embeddings_idx
VECTOR_KEY_PREFIX=doc:
```

## 技術仕様

- Embedding Model: `text-embedding-3-small` (1536次元)
- Distance Metric: Cosine Similarity
- Index Algorithm: FLAT
- Vector Type: FLOAT32