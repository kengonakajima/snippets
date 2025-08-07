require('dotenv').config();
const { uploadMessages, listMessages } = require('./blob-storage');
const { uploadFile, listFiles } = require('./file-storage');

async function main() {
    console.log('=== Azure Storage Sample Application ===\n');
    
    if (!process.env.AZURE_STORAGE_CONNECTION_STRING) {
        console.error('Error: AZURE_STORAGE_CONNECTION_STRING not found in environment variables.');
        console.error('Please create a .env file based on .env.example and add your connection string.');
        process.exit(1);
    }
    
    console.log('1. Blob Storage Operations\n');
    console.log('---------------------------');
    await uploadMessages();
    await listMessages();
    
    console.log('\n2. File Storage Operations\n');
    console.log('---------------------------');
    await uploadFile('./route.png');
    await listFiles();
    
    console.log('\n=== All operations completed successfully ===');
}

main().catch((err) => {
    console.error('Error in main:', err);
    process.exit(1);
});