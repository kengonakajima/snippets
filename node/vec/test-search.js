import { createEmbedding } from './embeddings.js';
import { 
    getRedisClient, 
    searchSimilar, 
    closeConnection 
} from './redisClient.js';

async function testSearch() {
    console.log('🧪 Testing Vector Search\n');
    
    await getRedisClient();
    
    const testQueries = [
        { query: 'wine with high acidity and citrus notes', category: null },
        { query: 'artificial intelligence and machine learning', category: null },
        { query: 'comfortable chair for office work', category: null },
        { query: 'climate change and global warming', category: null }
    ];
    
    for (const test of testQueries) {
        console.log(`\n📝 Query: "${test.query}"`);
        if (test.category) {
            console.log(`   Category filter: ${test.category}`);
        }
        console.log('───────────────────────────────────────────────────────────────────');
        
        const queryEmbedding = await createEmbedding(test.query);
        const results = await searchSimilar(queryEmbedding, 3, test.category);
        
        if (results.length === 0) {
            console.log('   No results found.');
        } else {
            results.forEach((result, index) => {
                console.log(`\n   ${index + 1}. [${result.category}] ${result.id}`);
                console.log(`      Similarity: ${(1 - result.score).toFixed(4)}`);
                console.log(`      Text: "${result.text.substring(0, 100)}..."`);
            });
        }
    }
    
    await closeConnection();
    console.log('\n\n✅ Test complete!');
}

testSearch().catch(err => {
    console.error('❌ Test failed:', err);
    process.exit(1);
});