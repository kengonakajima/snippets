# 0) 変数に安全に読み込む（画面に出さない）
read -r -s API_KEY   # パスワード入力プロンプトあり（入力は非表示）
echo                 # 改行だけ出す（見た目用）

# 1) 保存（login.keychain に Generic Password を作成）
security add-generic-password \
  -a "hoge" \
  -s "openai" \
  -D "application password" \
  -l "OpenAI API Key" \
  -w "$API_KEY" \
  -U
# -a: アカウント名、-s: サービス名、-l: ラベル名、-w: シークレット値、-U: 既存なら上書き

# 2) 取得（-w で値のみ出力）→ 環境変数に格納
export OPENAI_API_KEY="$(security find-generic-password -a hoge -s openai -w 2>/dev/null)"

# 取得したキーを表示
echo "取得したキー: $OPENAI_API_KEY"

# 3) 削除（不要になったら）
security delete-generic-password -a hoge -s openai

