# ElasticSearch 特許検索会話ログ

## 1. インデックス一覧の確認

**コマンド:**
```bash
curl -X GET "localhost:9200/_cat/indices?v"
```

**結果:**
- **patents_jp_20210201**: 日本の特許データのインデックス（2021年2月1日版）
  - ドキュメント数: 273,758,562件
  - サイズ: 1.1TB
  - ステータス: green（健全）

## 2. 納豆菌の寿命に関する特許検索

**検索クエリ:**
```json
{
  "query": {
    "bool": {
      "must": [
        {
          "multi_match": {
            "query": "納豆菌",
            "fields": ["title", "abstract", "description"]
          }
        },
        {
          "multi_match": {
            "query": "寿命",
            "fields": ["title", "abstract", "description"]
          }
        }
      ]
    }
  },
  "size": 10,
  "_source": ["title", "abstract", "application_number", "publication_date"]
}
```

**検索結果（1件）:**
- **特許番号**: JP2000128680
- **タイトル**: 生ごみ処理機の新型長寿命かつ無臭の菌床の発明
- **要約**: 食品残渣物をバイオ分解処理する際の問題（悪臭、発錆摩耗、バイオ菌活動力低下・死滅）を解決する発明。生ごみ処理用バイオ菌に木材チップと籾殻を混合した新混合体菌床を使用し、温度管理と撹拌制御により菌の長寿命化と無臭化を実現。

## 3. インデックスの総件数確認

**コマンド:**
```bash
curl -X GET "localhost:9200/patents_jp_20210201/_count"
```

**結果:** 14,251,866件（約1,425万件）

## 4. 人工光合成関連特許の件数

**検索クエリ:**
```json
{
  "query": {
    "multi_match": {
      "query": "人工光合成",
      "fields": ["title", "abstract", "description"]
    }
  }
}
```

**結果:** 16,162件

## 5. 人工光合成関連特許の最新3件

**検索クエリ:**
```json
{
  "query": {
    "multi_match": {
      "query": "人工光合成",
      "fields": ["title", "abstract", "description"]
    }
  },
  "size": 3,
  "sort": [
    {
      "publishedDate.iso": {
        "order": "desc"
      }
    }
  ],
  "_source": ["title", "abstract", "documentId", "publishedDate", "kind"]
}
```

**最新3件の結果:**

1. **JP2021500155** (2021年1月7日公開)
   - タイトル: 人工弁尖
   - PDFリンク: https://www.j-platpat.inpit.go.jp/c1800/PU/JP-2021-500155/15/ja

2. **JP2021500310** (2021年1月7日公開)
   - タイトル: 薬物および薬物製剤を調べるための人工硝子体液
   - PDFリンク: https://www.j-platpat.inpit.go.jp/c1800/PU/JP-2021-500310/15/ja

3. **JP2021000917** (2021年1月7日公開)
   - タイトル: 宇宙医療装置、宇宙医療方法、人工衛星
   - PDFリンク: https://www.j-platpat.inpit.go.jp/c1800/PU/JP-2021-000917/15/ja

## 使用したElasticSearchエンドポイント

- インデックス一覧: `GET /_cat/indices?v`
- 文書数カウント: `GET /patents_jp_20210201/_count`
- 検索: `GET /patents_jp_20210201/_search`
- マッピング確認: `GET /patents_jp_20210201/_mapping`

## データベース情報

- **サーバー**: localhost:9200
- **インデックス**: patents_jp_20210201
- **総文書数**: 14,251,866件
- **データサイズ**: 1.1TB
- **主要検索フィールド**: title, abstract, description