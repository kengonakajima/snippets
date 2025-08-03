# GitHub App リポジトリリスト取得サンプル

GitHub Appを使用して組織のリポジトリリストを取得するNode.jsサンプルコードです。

## セットアップ

### 1. GitHub Appの作成

1. GitHubの Settings > Developer settings > GitHub Apps > New GitHub App
2. 以下の設定を行う：
   - **GitHub App name**: 任意の名前
   - **Homepage URL**: 任意のURL
   - **Webhook**: Active のチェックを外す（このサンプルでは不要）
   - **Permissions**: 
     - Repository permissions > Contents: Read
     - Repository permissions > Metadata: Read
     - Organization permissions > Members: Read（オプション）
   - **Where can this GitHub App be installed?**: Only on this account

3. 作成後、以下を取得：
   - **App ID**: Generalページに表示
   - **Private Key**: "Generate a private key"ボタンから生成

### 2. GitHub Appのインストール

1. GitHub Appの設定ページから "Install App" を選択
2. 対象の組織を選択してインストール
3. アクセスを許可するリポジトリを選択（All repositoriesまたは特定のリポジトリ）

### 3. 環境設定

```bash
# 依存関係のインストール
npm install

# 環境変数の設定
cp .env.example .env
# .envファイルを編集して、実際の値を設定
```

**注意**: このサンプルはdotenvを使用して`.env`ファイルから環境変数を自動的に読み込みます。

### 4. 実行

```bash
npm start
# または
node github-app-repos.js
```

## 環境変数

- `GITHUB_APP_ID`: GitHub AppのApp ID
- `GITHUB_APP_PRIVATE_KEY`: GitHub Appの秘密鍵（PEM形式）
- `GITHUB_ORG`: 対象の組織名

## Private Keyの設定方法

Private Keyは改行を含むため、環境変数に設定する際は以下のいずれかの方法を使用：

### 方法1: 改行を\nに置換
```bash
GITHUB_APP_PRIVATE_KEY="-----BEGIN RSA PRIVATE KEY-----\nMIIEpAIBAAKCAQEA...\n-----END RSA PRIVATE KEY-----"
```

### 方法2: ファイルから読み込む
```javascript
const PRIVATE_KEY = fs.readFileSync('path/to/private-key.pem', 'utf8');
```

## 取得できる情報

- リポジトリ名
- 説明
- プライベート/パブリック
- 最終更新日
- 主要言語
- スター数

## トラブルシューティング

### "GitHub App is not installed for organization" エラー
→ GitHub Appが対象組織にインストールされていません。インストール手順を確認してください。

### 認証エラー
→ App IDまたはPrivate Keyが正しく設定されているか確認してください。

### 権限エラー
→ GitHub Appの権限設定でRepository permissionsが適切に設定されているか確認してください。