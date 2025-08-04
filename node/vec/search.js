import readline from 'readline';
import { createEmbedding } from './embeddings.js';
import { 
    getRedisClient, 
    searchSimilar, 
    closeConnection 
} from './redisClient.js';

const rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout
});

function prompt(question) {
    return new Promise((resolve) => {
        rl.question(question, resolve);
    });
}

async function performSearch() {
    console.log('🔍 Vector Search Demo\n');
    console.log('Available categories: Wine, Technology, Environment, Finance, Health, Space,');
    console.log('                     Agriculture, Energy, Education, Urban Development,');
    console.log('                     Electronics, Furniture, Sports, Lifestyle, Home Security,');
    console.log('                     Kitchen, Outdoor, Beauty, Gaming, Fitness\n');
    
    await getRedisClient();
    
    while (true) {
        const query = await prompt('\n📝 Enter search query (or "quit" to exit): ');
        
        if (query.toLowerCase() === 'quit') {
            break;
        }
        
        const category = await prompt('🏷️  Filter by category (press Enter to skip): ');
        const topK = await prompt('🔢 Number of results (default 5): ');
        
        console.log('\n⏳ Searching...');
        
        const queryEmbedding = await createEmbedding(query);
        
        const results = await searchSimilar(
            queryEmbedding,
            parseInt(topK) || 5,
            category || null
        );
        
        console.log('\n📊 Search Results:');
        console.log('═══════════════════════════════════════════════════════════════════\n');
        
        if (results.length === 0) {
            console.log('No results found.');
        } else {
            results.forEach((result, index) => {
                console.log(`${index + 1}. [${result.category}] ${result.id}`);
                console.log(`   Score: ${(1 - result.score).toFixed(4)} (similarity)`);
                console.log(`   Text: ${result.text.substring(0, 100)}...`);
                console.log();
            });
        }
    }
    
    rl.close();
    await closeConnection();
    console.log('\n👋 Goodbye!');
}

performSearch().catch(err => {
    console.error('❌ Search failed:', err);
    rl.close();
    process.exit(1);
});