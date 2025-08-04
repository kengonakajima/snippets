import fs from 'fs/promises';
import path from 'path';
import fetchSampleData from './fetchData.js';
import { createEmbedding } from './embeddings.js';
import { 
    getRedisClient, 
    createVectorIndex, 
    storeDocument, 
    closeConnection 
} from './redisClient.js';

async function setupVectorDatabase() {
    console.log('🚀 Starting Vector Database Setup...\n');
    
    console.log('📥 Fetching sample data...');
    await fetchSampleData();
    
    const dataPath = path.join('./data', 'sample_documents.json');
    const documentsData = await fs.readFile(dataPath, 'utf-8');
    const documents = JSON.parse(documentsData);
    
    console.log(`\n📊 Processing ${documents.length} documents...`);
    
    await getRedisClient();
    
    console.log('\n🔧 Creating vector index in Redis...');
    await createVectorIndex(1536);
    
    console.log('\n🧮 Creating embeddings and storing in Redis...');
    
    for (let i = 0; i < documents.length; i++) {
        const doc = documents[i];
        process.stdout.write(`\rProcessing document ${i + 1}/${documents.length}: ${doc.id}`);
        
        const embedding = await createEmbedding(doc.text);
        
        await storeDocument({
            id: doc.id,
            text: doc.text,
            category: doc.category,
            embedding: embedding
        });
        
        if (i === 0) {
            console.log(`\n   Embedding dimension: ${embedding.length}`);
        }
    }
    
    console.log('\n\n✅ Setup complete!');
    console.log(`   - ${documents.length} documents processed`);
    console.log('   - Vector index created');
    console.log('   - Documents stored in Redis\n');
    
    await closeConnection();
}

setupVectorDatabase().catch(err => {
    console.error('❌ Setup failed:', err);
    process.exit(1);
});