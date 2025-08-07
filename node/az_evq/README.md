# Azure Storage Queue Event Processor

Azure Storage Queueからイベントを取得し、ファイルの変更を検知してダウンロードするサンプルコードです。

## セットアップ

1. 依存関係のインストール
```bash
npm install
```

2. 環境変数の設定
`.env.sample`を`.env`にコピーして、Azure Storage Accountの接続情報を設定してください。

```bash
cp .env.sample .env
```

`.env`ファイルを編集:
- `AZURE_STORAGE_CONNECTION_STRING`: Azure Storage Accountの接続文字列
- `QUEUE_NAME`: 監視するキューの名前
- `BLOB_CONTAINER_NAME`: ダウンロード対象のコンテナ名（イベントから自動検出されない場合のデフォルト）

## 実行

```bash
npm start
```

## 機能

- Azure Storage Queueからメッセージを継続的にポーリング
- 受信したイベントの内容を表示
- Storage Blob作成・更新イベントを検知
- 変更されたファイルを`./downloads`ディレクトリに自動ダウンロード
- Event Grid形式とカスタム形式の両方のイベントに対応

## 対応するイベント形式

1. **Event Grid形式**: Microsoft.Storage.BlobCreated/BlobModified
2. **カスタム形式**: blobName/fileNameフィールドを含むJSON