import OpenAI from 'openai';
import dotenv from 'dotenv';

dotenv.config();

const openai = new OpenAI({
    apiKey: process.env.OPENAI_API_KEY,
});

async function createEmbedding(text, model = 'text-embedding-3-small') {
    const response = await openai.embeddings.create({
        input: text,
        model: model,
    });
    
    return response.data[0].embedding;
}

async function createEmbeddings(texts, model = 'text-embedding-3-small') {
    const response = await openai.embeddings.create({
        input: texts,
        model: model,
    });
    
    return response.data.map(item => item.embedding);
}

export { createEmbedding, createEmbeddings };