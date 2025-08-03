# Workload Identity連携のローカル検証手順

## 方法1: gcloud認証を使用（最も簡単）

```bash
# 1. gcloud CLIがインストールされていることを確認
gcloud --version

# 2. デフォルトアプリケーション認証を設定
gcloud auth application-default login

# 3. sa-wif.jsを実行
node sa-wif.js
```

この方法では、あなたのGoogleアカウントの権限でDriveにアクセスします。

## 方法2: サービスアカウントのなりすまし（Impersonation）

```bash
# 1. サービスアカウントを作成（Google Cloud Console）
# 2. 必要なロールを付与（Drive API アクセス権限）

# 3. サービスアカウントになりすます
gcloud auth application-default login --impersonate-service-account=SERVICE_ACCOUNT_EMAIL

# 4. sa-wif.jsを実行
node sa-wif.js
```

## 方法3: ローカルでWorkload Identityをシミュレート

```bash
# 1. 環境変数でサービスアカウントキーを指定（一時的な検証用）
export GOOGLE_APPLICATION_CREDENTIALS=./service-account-key.json

# 2. sa-wif.jsを実行
node sa-wif.js
```

## CI/CD環境への移行準備

ローカルで動作確認後、以下の設定でCI/CDに移行できます：

1. **Workload Identity Poolの作成**
2. **外部IDプロバイダーの設定**（GitHub、GitLab等）
3. **サービスアカウントへの権限付与**
4. **CI/CD環境での認証設定**

## 注意事項

- `sa-wif.js`は認証方法を自動検出するため、上記のどの方法でも動作します
- ローカルでの検証後、同じコードをCI/CD環境で使用できます
- Workload Identity連携の本来の利点（キーレス認証）はCI/CD環境で発揮されます