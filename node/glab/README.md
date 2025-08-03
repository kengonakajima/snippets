# GitLab Repository Fetcher

GitLabのPersonal Access Tokenを使用して、組織（グループ）内の全リポジトリリストを取得するNode.jsサンプルコードです。

## セットアップ

1. 依存関係のインストール
```bash
npm install
```

2. 環境変数の設定
`.env.sample`をコピーして`.env`ファイルを作成し、必要な値を設定します：

```bash
cp .env.sample .env
```

`.env`ファイルを編集：
```
GITLAB_ACCESS_TOKEN=your_personal_access_token
GITLAB_URL=https://gitlab.com  # または自社のGitLabインスタンスURL
GITLAB_GROUP_ID=your_group_id_or_path
```

## Personal Access Tokenの取得方法

1. GitLabにログイン
2. ユーザー設定 → Access Tokens
3. 「Add new token」をクリック
4. Token name: 任意の名前（例: "API Access"）
5. Expiration date: 有効期限を設定
6. Select scopes: 最低限「read_api」にチェック
7. 「Create personal access token」をクリック
8. 表示されたトークンを安全な場所に保存（再表示できません）

**重要**: Feed tokenはAPIアクセスには使用できません。必ずPersonal Access Tokenを使用してください。

## 使用方法

```bash
npm start
```

または

```bash
node index.js
```

## 出力内容

- リポジトリ名
- パス（namespace付き）
- WebのURL
- 説明
- 作成日
- 最終更新日
- 使用言語の統計