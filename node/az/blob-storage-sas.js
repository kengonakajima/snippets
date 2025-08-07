require('dotenv').config();
const { BlobServiceClient } = require('@azure/storage-blob');

// SASトークンを使用した接続（アップロード専用）
const sasToken = process.env.AZURE_STORAGE_SAS_TOKEN;
const accountName = 'nandemo';
const containerName = 'messages';

async function uploadMessagesWithSAS() {
    const blobServiceClient = new BlobServiceClient(
        `https://${accountName}.blob.core.windows.net?${sasToken}`
    );
    const containerClient = blobServiceClient.getContainerClient(containerName);
    
    console.log('Using SAS token for upload-only access');
    
    for (let i = 11; i <= 15; i++) {
        const blobName = `messages/message-${i}.json`;
        const blockBlobClient = containerClient.getBlockBlobClient(blobName);
        
        const data = JSON.stringify({
            type: "hello-sas",
            count: i
        }, null, 2);
        
        const uploadResponse = await blockBlobClient.upload(data, data.length);
        console.log(`Uploaded blob ${blobName} successfully via SAS`, uploadResponse.requestId);
    }
}

async function main() {
    console.log('Starting Blob Storage operations with SAS token...\n');
    
    if (!sasToken) {
        console.error('Error: AZURE_STORAGE_SAS_TOKEN not found in environment variables.');
        console.error('Please add the SAS token to your .env file');
        process.exit(1);
    }
    
    await uploadMessagesWithSAS();
}

if (require.main === module) {
    main().catch((err) => {
        console.error('Error:', err.message);
        if (err.statusCode === 403) {
            console.error('Permission denied - check your SAS token permissions');
        }
        process.exit(1);
    });
}

module.exports = { uploadMessagesWithSAS };