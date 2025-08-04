#!/bin/bash

# .envファイルからAPIキーを読み込む
export $(cat .env | xargs)

echo "Testing Fireworks.ai API directly with curl..."

# curlでAPIを直接呼び出し
curl -X POST https://api.fireworks.ai/inference/v1/embeddings \
  -H "Authorization: Bearer $FIREWORKS_API_KEY" \
  -H "Content-Type: application/json" \
  -d '{
    "model": "nomic-ai/nomic-embed-text-v1.5",
    "input": "search_document: Spiderman was a particularly entertaining movie with great action scenes",
    "dimensions": 768
  }' | python3 -m json.tool