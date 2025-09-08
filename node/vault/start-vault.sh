#!/bin/bash

# Vaultサーバーを開発モードで起動するスクリプト
export VAULT_ADDR='http://127.0.0.1:8200'

echo "🔐 Vaultサーバーを起動しています..."
echo "   アドレス: $VAULT_ADDR"
echo "   ルートトークン: dev-token"
echo ""
echo "停止するには Ctrl+C を押してください"
echo ""

./vault server -dev -dev-root-token-id=dev-token