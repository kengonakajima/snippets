import { createEmbedding } from './embeddings.js';
import { 
    getRedisClient, 
    createVectorIndex,
    storeDocument,
    searchSimilar, 
    closeConnection 
} from './redisClient.js';

async function demonstrateVectorSearch() {
    console.log('🎯 Vector Search Demonstration\n');
    console.log('═══════════════════════════════════════════════════════════════════\n');
    
    await getRedisClient();
    
    const demoDocuments = [
        {
            id: 'demo_1',
            text: 'Machine learning algorithms can analyze patterns in large datasets to make predictions and automate decision-making processes.',
            category: 'AI'
        },
        {
            id: 'demo_2',
            text: 'Neural networks are inspired by the human brain and consist of interconnected nodes that process information in layers.',
            category: 'AI'
        },
        {
            id: 'demo_3',
            text: 'Climate change is causing rising sea levels and extreme weather events that threaten coastal communities worldwide.',
            category: 'Climate'
        }
    ];
    
    console.log('📚 Creating demo documents and embeddings...\n');
    
    for (const doc of demoDocuments) {
        console.log(`   Processing: ${doc.id} (${doc.category})`);
        const embedding = await createEmbedding(doc.text);
        await storeDocument({
            ...doc,
            embedding: embedding
        });
    }
    
    console.log('\n🔍 Performing similarity searches...\n');
    
    const queries = [
        'artificial intelligence and deep learning',
        'global warming and environmental impact',
        'brain-inspired computing systems'
    ];
    
    for (const query of queries) {
        console.log(`Query: "${query}"`);
        console.log('───────────────────────────────────────────────────────────────────');
        
        const queryEmbedding = await createEmbedding(query);
        const results = await searchSimilar(queryEmbedding, 3);
        
        results.forEach((result, index) => {
            console.log(`  ${index + 1}. [${result.category}] ${result.id}`);
            console.log(`     Similarity: ${(1 - result.score).toFixed(4)}`);
            console.log(`     Text: ${result.text.substring(0, 80)}...`);
        });
        
        console.log();
    }
    
    await closeConnection();
    console.log('✅ Demonstration complete!\n');
    console.log('To set up the full database, run: npm run setup');
    console.log('To search interactively, run: npm run search');
}

demonstrateVectorSearch().catch(err => {
    console.error('❌ Error:', err);
    process.exit(1);
});