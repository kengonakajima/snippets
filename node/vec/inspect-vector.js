import { createEmbedding } from './embeddings.js';
import { getRedisClient, closeConnection } from './redisClient.js';

async function inspectVectorStorage() {
    console.log('🔍 Vector Data Inspection\n');
    
    // 1. OpenAIから取得した生のベクトル
    const testText = "Hello world";
    const embedding = await createEmbedding(testText);
    
    console.log('1️⃣ OpenAI Embedding (JavaScript Array):');
    console.log(`   Type: ${typeof embedding}`);
    console.log(`   Is Array: ${Array.isArray(embedding)}`);
    console.log(`   Length: ${embedding.length}`);
    console.log(`   First 5 values: [${embedding.slice(0, 5).map(v => v.toFixed(4)).join(', ')}...]`);
    console.log(`   Memory size: ~${embedding.length * 8} bytes (as JS numbers)\n`);
    
    // 2. Float32Arrayへの変換
    const float32Array = new Float32Array(embedding);
    console.log('2️⃣ Float32Array:');
    console.log(`   Type: ${float32Array.constructor.name}`);
    console.log(`   Length: ${float32Array.length}`);
    console.log(`   Bytes per element: ${float32Array.BYTES_PER_ELEMENT}`);
    console.log(`   Total byte length: ${float32Array.byteLength} bytes`);
    console.log(`   First 5 values: [${Array.from(float32Array.slice(0, 5)).map(v => v.toFixed(4)).join(', ')}...]\n`);
    
    // 3. Bufferへの変換（バイナリ）
    const buffer = Buffer.from(float32Array.buffer);
    console.log('3️⃣ Buffer (Binary):');
    console.log(`   Type: ${buffer.constructor.name}`);
    console.log(`   Length: ${buffer.length} bytes`);
    console.log(`   Is Buffer: ${Buffer.isBuffer(buffer)}`);
    console.log(`   First 20 bytes (hex): ${buffer.slice(0, 20).toString('hex')}`);
    console.log(`   First 20 bytes (raw): [${Array.from(buffer.slice(0, 20)).join(', ')}]\n`);
    
    // 4. Redisから取得して確認
    const client = await getRedisClient();
    
    // サンプルドキュメントを保存
    await client.hSet('doc:test_binary', {
        id: 'test',
        text: testText,
        category: 'Test',
        embedding: buffer
    });
    
    // 生のデータを取得
    const storedData = await client.hGet('doc:test_binary', 'embedding');
    console.log('4️⃣ Retrieved from Redis:');
    console.log(`   Type: ${storedData.constructor.name}`);
    console.log(`   Is Buffer: ${Buffer.isBuffer(storedData)}`);
    console.log(`   Length: ${storedData.length} bytes`);
    console.log(`   First 20 bytes (hex): ${storedData.slice(0, 20).toString('hex')}`);
    console.log(`   Matches original: ${Buffer.compare(buffer, storedData) === 0}\n`);
    
    // 5. バイナリから復元
    const restoredFloat32 = new Float32Array(storedData.buffer, storedData.byteOffset, storedData.byteLength / 4);
    console.log('5️⃣ Restored from Binary:');
    console.log(`   First 5 values: [${Array.from(restoredFloat32.slice(0, 5)).map(v => v.toFixed(4)).join(', ')}...]`);
    console.log(`   Matches original: ${JSON.stringify(Array.from(float32Array.slice(0, 5))) === JSON.stringify(Array.from(restoredFloat32.slice(0, 5)))}\n`);
    
    // サイズ比較
    console.log('📊 Size Comparison:');
    console.log(`   JSON string: ~${JSON.stringify(embedding).length} bytes`);
    console.log(`   Binary (Float32): ${buffer.length} bytes`);
    console.log(`   Compression ratio: ${(JSON.stringify(embedding).length / buffer.length).toFixed(2)}x larger as JSON\n`);
    
    // クリーンアップ
    await client.del('doc:test_binary');
    await closeConnection();
}

inspectVectorStorage().catch(console.error);