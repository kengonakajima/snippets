# SQLite-Vec Vector Search Demo

Node.jsでsqlite-vecとOpenAI Embeddingsを使用したベクトル検索のデモアプリケーション。

## セットアップ

### 1. 依存関係のインストール

```bash
npm install
```

### 2. OpenAI APIキーの設定

`.env`ファイルを作成し、OpenAI APIキーを設定：

```bash
echo "OPENAI_API_KEY=your_api_key_here" > .env
```

## 使い方

### ドキュメントの追加

```bash
node index.js add "Node.jsは非同期処理が得意なJavaScriptランタイムです"
node index.js add "Pythonは機械学習やデータ分析でよく使われる言語です"
node index.js add "JavaScriptはWebブラウザで動作するプログラミング言語です"
```

### 類似検索

```bash
node index.js search "プログラミング言語について"
node index.js search "非同期処理" --limit 3
```

### ドキュメント一覧

```bash
node index.js list
```

### ドキュメントの削除

```bash
node index.js delete 1
```

### 全ドキュメントのクリア

```bash
node index.js clear
```

## コマンド一覧

- `add <text>` - テキストをデータベースに追加
- `search <query> [--limit N]` - 類似ドキュメントを検索（デフォルト5件）
- `list` - 全ドキュメントを表示
- `delete <id>` - 指定IDのドキュメントを削除
- `clear` - 全ドキュメントを削除

## 技術仕様

- **埋め込みモデル**: OpenAI text-embedding-3-small (1536次元)
- **類似度計算**: コサイン類似度
- **データベース**: SQLite + sqlite-vec拡張
- **データベースファイル**: `./vector_demo.db`

## ファイル構成

- `index.js` - メインCLIアプリケーション
- `db.js` - データベース操作
- `embeddings.js` - OpenAI埋め込み処理
- `search.js` - ベクトル検索処理
- `spec.txt` - 詳細仕様書

## 注意事項

- OpenAI APIの利用には料金が発生します
- 大量のテキストを処理する場合はAPI制限に注意してください