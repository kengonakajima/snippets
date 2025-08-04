import { createClient, SchemaFieldTypes, VectorAlgorithms } from 'redis';
import dotenv from 'dotenv';

dotenv.config();

let redisClient = null;

async function getRedisClient() {
    if (redisClient) {
        return redisClient;
    }

    redisClient = createClient({
        socket: {
            host: process.env.REDIS_HOST || 'localhost',
            port: parseInt(process.env.REDIS_PORT || '6379')
        },
        password: process.env.REDIS_PASSWORD || undefined,
        database: parseInt(process.env.REDIS_DB || '0')
    });

    redisClient.on('error', (err) => {
        console.error('Redis Client Error:', err);
    });

    await redisClient.connect();
    console.log('Connected to Redis');
    
    return redisClient;
}

async function createVectorIndex(dimensions = 1536) {
    const client = await getRedisClient();
    const indexName = process.env.VECTOR_INDEX_NAME || 'embeddings_idx';
    const keyPrefix = process.env.VECTOR_KEY_PREFIX || 'doc:';
    
    const schema = {
        'id': {
            type: SchemaFieldTypes.TEXT
        },
        'text': {
            type: SchemaFieldTypes.TEXT
        },
        'category': {
            type: SchemaFieldTypes.TAG
        },
        'embedding': {
            type: SchemaFieldTypes.VECTOR,
            ALGORITHM: VectorAlgorithms.FLAT,
            TYPE: 'FLOAT32',
            DIM: dimensions,
            DISTANCE_METRIC: 'COSINE'
        }
    };

    try {
        await client.ft.dropIndex(indexName);
        console.log(`Dropped existing index: ${indexName}`);
    } catch (err) {
        console.log('No existing index to drop');
    }

    await client.ft.create(
        indexName,
        schema,
        {
            ON: 'HASH',
            PREFIX: keyPrefix
        }
    );
    
    console.log(`Created vector index: ${indexName}`);
    return indexName;
}

async function storeDocument(doc) {
    const client = await getRedisClient();
    const keyPrefix = process.env.VECTOR_KEY_PREFIX || 'doc:';
    const key = `${keyPrefix}${doc.id}`;
    
    const float32Array = new Float32Array(doc.embedding);
    const buffer = Buffer.from(float32Array.buffer);
    
    await client.hSet(key, {
        id: doc.id,
        text: doc.text,
        category: doc.category,
        embedding: buffer
    });
    
    return key;
}

async function searchSimilar(queryEmbedding, topK = 5, category = null) {
    const client = await getRedisClient();
    const indexName = process.env.VECTOR_INDEX_NAME || 'embeddings_idx';
    
    const float32Array = new Float32Array(queryEmbedding);
    const buffer = Buffer.from(float32Array.buffer);
    
    let query = `*=>[KNN ${topK} @embedding $BLOB AS score]`;
    
    if (category) {
        const escapedCategory = category.replace(/[\s\-]/g, '\\$&');
        query = `(@category:{${escapedCategory}}) ${query}`;
    }
    
    const results = await client.ft.search(
        indexName,
        query,
        {
            PARAMS: {
                BLOB: buffer
            },
            SORTBY: {
                BY: 'score',
                DIRECTION: 'ASC'
            },
            RETURN: ['id', 'text', 'category', 'score'],
            DIALECT: 2
        }
    );
    
    return results.documents.map(doc => ({
        id: doc.value.id,
        text: doc.value.text,
        category: doc.value.category,
        score: parseFloat(doc.value.score)
    }));
}

async function closeConnection() {
    if (redisClient) {
        await redisClient.quit();
        redisClient = null;
        console.log('Disconnected from Redis');
    }
}

export {
    getRedisClient,
    createVectorIndex,
    storeDocument,
    searchSimilar,
    closeConnection
};