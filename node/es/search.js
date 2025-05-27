#!/usr/bin/env node

const { Client } = require('@elastic/elasticsearch');

// Elasticsearchクライアントの初期化
const client = new Client({
  node: 'http://localhost:9200'
});

async function search(query) {
  try {
    // Elasticsearchへの接続確認
    await client.ping();
    
    if (!query || query.trim() === '') {
      console.log('使用方法: node search.js <検索語>');
      process.exit(1);
    }

    // 検索実行
    const response = await client.search({
      index: 'products',
      body: {
        query: {
          multi_match: {
            query: query,
            fields: ['name^2', 'description', 'brand', 'category', 'specs.*'],
            fuzziness: 'AUTO'
          }
        },
        highlight: {
          fields: {
            'name': {},
            'description': {},
            'brand': {},
            'category': {}
          }
        },
        sort: [
          { _score: { order: 'desc' } },
          { price: { order: 'asc' } }
        ]
      }
    });

    console.log(`\n検索語: "${query}"`);
    console.log(`検索結果: ${response.hits.total.value}件\n`);

    if (response.hits.hits.length === 0) {
      console.log('該当する商品が見つかりませんでした。');
      return;
    }

    // 結果表示
    response.hits.hits.forEach((hit, index) => {
      const source = hit._source;
      console.log(`${index + 1}. ${source.name} (スコア: ${hit._score.toFixed(2)})`);
      console.log(`   ブランド: ${source.brand}`);
      console.log(`   カテゴリ: ${source.category}`);
      console.log(`   価格: ¥${source.price.toLocaleString()}`);
      console.log(`   在庫: ${source.in_stock ? '有り' : '無し'}`);
      console.log(`   説明: ${source.description}`);
      
      // ハイライト表示
      if (hit.highlight) {
        console.log('   マッチ箇所:');
        Object.entries(hit.highlight).forEach(([field, highlights]) => {
          highlights.forEach(highlight => {
            console.log(`     ${field}: ${highlight}`);
          });
        });
      }
      
      console.log('');
    });

  } catch (error) {
    if (error.meta && error.meta.statusCode === 404) {
      console.error('エラー: "products"インデックスが見つかりません。');
      console.error('Elasticsearchにデータがインデックスされているか確認してください。');
    } else if (error.name === 'ConnectionError') {
      console.error('エラー: Elasticsearchに接続できません。');
      console.error('localhost:9200でElasticsearchが起動しているか確認してください。');
    } else {
      console.error('検索エラー:', error.message);
    }
    process.exit(1);
  }
}

// コマンドライン引数から検索語を取得
const searchQuery = process.argv.slice(2).join(' ');
search(searchQuery);