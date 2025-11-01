#!/bin/bash

# systemd-ask-passwordでパスワードを取得
PASSWORD=$(systemd-ask-password "Please enter password:")

# 環境変数に設定してhoge.jsを実行
export MY_PASSWORD="$PASSWORD"
node hoge.js

# セキュリティのため環境変数をクリア
unset MY_PASSWORD
