require('dotenv').config();
const { BlobServiceClient } = require('@azure/storage-blob');

const connectionString = process.env.AZURE_STORAGE_CONNECTION_STRING;
const containerName = 'messages';

async function uploadMessages() {
    const blobServiceClient = BlobServiceClient.fromConnectionString(connectionString);
    const containerClient = blobServiceClient.getContainerClient(containerName);
    
    await containerClient.createIfNotExists();
    console.log(`Container "${containerName}" is ready.`);
    
    for (let i = 1; i <= 10; i++) {
        const blobName = `messages/message-${i}.json`;
        const blockBlobClient = containerClient.getBlockBlobClient(blobName);
        
        const data = JSON.stringify({
            type: "hello",
            count: i
        }, null, 2);
        
        const uploadResponse = await blockBlobClient.upload(data, data.length);
        console.log(`Uploaded blob ${blobName} successfully`, uploadResponse.requestId);
    }
}

async function listMessages() {
    const blobServiceClient = BlobServiceClient.fromConnectionString(connectionString);
    const containerClient = blobServiceClient.getContainerClient(containerName);
    
    console.log('\nListing blobs:');
    for await (const blob of containerClient.listBlobsFlat({ prefix: 'messages/' })) {
        console.log(`- Name: ${blob.name}, Size: ${blob.properties.contentLength} bytes`);
    }
}

async function main() {
    console.log('Starting Blob Storage operations...\n');
    await uploadMessages();
    await listMessages();
}

if (require.main === module) {
    main().catch((err) => {
        console.error('Error:', err);
        process.exit(1);
    });
}

module.exports = { uploadMessages, listMessages };