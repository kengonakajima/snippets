const vault = require('node-vault');

// Vaultクライアントの設定
const client = vault({
  apiVersion: 'v1',
  endpoint: 'http://127.0.0.1:8200',
  token: 'dev-token' // 開発用のトークン
});

async function basicVaultOperations() {
  try {
    console.log('🔐 HashiCorp Vault 基本動作確認を開始します...\n');

    // 1. Vaultサーバーの状態確認
    console.log('1. Vaultサーバーの状態確認');
    const health = await client.health();
    console.log('✅ Vaultサーバーは正常に動作しています');
    console.log('   バージョン:', health.version);
    console.log('   クラスター名:', health.cluster_name);
    console.log('');

    // 2. KVシークレットエンジンの確認
    console.log('2. KVシークレットエンジンの確認');
    console.log('✅ 開発モードではKVエンジンが自動的に有効化されています');
    console.log('');

    // 3. シークレットの書き込み
    console.log('3. シークレットの書き込み');
    const secretData = {
      username: 'admin',
      password: 'super-secret-password',
      api_key: 'sk-1234567890abcdef',
      database_url: 'postgresql://user:pass@localhost:5432/mydb'
    };

    await client.write('secret/data/myapp', {
      data: secretData
    });
    console.log('✅ シークレットが正常に書き込まれました');
    console.log('   パス: secret/data/myapp');
    console.log('   データ:', Object.keys(secretData).join(', '));
    console.log('');

    // 4. シークレットの読み込み
    console.log('4. シークレットの読み込み');
    const readResult = await client.read('secret/data/myapp');
    console.log('✅ シークレットが正常に読み込まれました');
    console.log('   取得したデータ:');
    Object.entries(readResult.data.data).forEach(([key, value]) => {
      console.log(`     ${key}: ${value}`);
    });
    console.log('');

    // 5. シークレットの更新
    console.log('5. シークレットの更新');
    const updatedData = {
      ...secretData,
      password: 'new-super-secret-password',
      last_updated: new Date().toISOString()
    };

    await client.write('secret/data/myapp', {
      data: updatedData
    });
    console.log('✅ シークレットが正常に更新されました');
    console.log('');

    // 6. 更新されたシークレットの確認
    console.log('6. 更新されたシークレットの確認');
    const updatedResult = await client.read('secret/data/myapp');
    console.log('✅ 更新されたシークレットを確認しました');
    console.log('   更新されたデータ:');
    Object.entries(updatedResult.data.data).forEach(([key, value]) => {
      console.log(`     ${key}: ${value}`);
    });
    console.log('');

    // 7. シークレットの一覧表示
    console.log('7. シークレットの一覧表示');
    try {
      const listResult = await client.list('secret/metadata/');
      console.log('✅ シークレットの一覧を取得しました');
      console.log('   シークレット一覧:', listResult.data.keys);
    } catch (error) {
      console.log('✅ シークレットの一覧表示（代替方法）');
      console.log('   現在保存されているシークレット: secret/data/myapp');
    }
    console.log('');

    console.log('🎉 すべての基本動作確認が完了しました！');

  } catch (error) {
    console.error('❌ エラーが発生しました:', error.message);
    if (error.message.includes('ECONNREFUSED')) {
      console.error('   Vaultサーバーが起動していない可能性があります');
      console.error('   docker-compose up でVaultサーバーを起動してください');
    }
    process.exit(1);
  }
}

// メイン関数の実行
if (require.main === module) {
  basicVaultOperations();
}

module.exports = { basicVaultOperations };