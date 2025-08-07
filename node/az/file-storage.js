require('dotenv').config();
const { ShareServiceClient } = require('@azure/storage-file-share');
const fs = require('fs');
const path = require('path');

const connectionString = process.env.AZURE_STORAGE_CONNECTION_STRING;
const shareName = 'myfiles';
const directoryName = 'images';

async function uploadFile(filePath) {
    const shareServiceClient = ShareServiceClient.fromConnectionString(connectionString);
    const shareClient = shareServiceClient.getShareClient(shareName);
    
    await shareClient.createIfNotExists();
    console.log(`File share "${shareName}" is ready.`);
    
    const directoryClient = shareClient.getDirectoryClient(directoryName);
    await directoryClient.createIfNotExists();
    console.log(`Directory "${directoryName}" is ready.`);
    
    const fileName = path.basename(filePath);
    const fileClient = directoryClient.getFileClient(fileName);
    
    const fileBuffer = fs.readFileSync(filePath);
    const fileSize = fileBuffer.length;
    
    await fileClient.create(fileSize);
    await fileClient.uploadRange(fileBuffer, 0, fileSize);
    
    console.log(`File "${fileName}" uploaded successfully. Size: ${fileSize} bytes`);
}

async function listFiles() {
    const shareServiceClient = ShareServiceClient.fromConnectionString(connectionString);
    const shareClient = shareServiceClient.getShareClient(shareName);
    const directoryClient = shareClient.getDirectoryClient(directoryName);
    
    console.log('\nListing files:');
    for await (const entity of directoryClient.listFilesAndDirectories()) {
        if (entity.kind === 'file') {
            console.log(`- Name: ${entity.name}, Size: ${entity.properties.contentLength} bytes`);
        }
    }
}

async function main() {
    console.log('Starting File Storage operations...\n');
    
    const filePath = './route.png';
    if (fs.existsSync(filePath)) {
        await uploadFile(filePath);
        await listFiles();
    } else {
        console.log(`File ${filePath} not found. Please create it first.`);
    }
}

if (require.main === module) {
    main().catch((err) => {
        console.error('Error:', err);
        process.exit(1);
    });
}

module.exports = { uploadFile, listFiles };