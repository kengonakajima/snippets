# Azure Storage Sample Application

Node.jsを使用してAzure StorageのBlobとFileにアクセスするサンプルコードです。

## 機能

### Blob Storage
- JSON形式のメッセージを10個連番で保存
- 保存したメッセージのリストを取得（名前とサイズ）

### File Storage  
- 画像ファイル（route.png）をアップロード
- アップロードしたファイルのリストを取得（名前とサイズ）

## セットアップ

1. 依存関係のインストール
```bash
npm install
```

2. 環境変数の設定
```bash
cp .env.example .env
```
`.env`ファイルを編集して、Azure Storage接続文字列を設定してください。

3. 実行
```bash
node index.js
# または個別に実行
node blob-storage.js
node file-storage.js
```

## ファイル構成

- `index.js` - メインエントリーポイント
- `blob-storage.js` - Blobストレージ操作
- `file-storage.js` - Fileストレージ操作
- `route.png` - サンプル画像ファイル
- `.env.example` - 環境変数のテンプレート