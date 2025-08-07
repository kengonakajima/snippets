require('dotenv').config();
const { QueueServiceClient } = require('@azure/storage-queue');
const { BlobServiceClient } = require('@azure/storage-blob');
const path = require('path');
const fs = require('fs').promises;

const connectionString = process.env.AZURE_STORAGE_CONNECTION_STRING;
const queueName = process.env.QUEUE_NAME;
const containerName = process.env.BLOB_CONTAINER_NAME;

async function processQueueMessages() {
    if (!connectionString || !queueName) {
        console.error('Error: AZURE_STORAGE_CONNECTION_STRING and QUEUE_NAME must be set in .env file');
        process.exit(1);
    }

    const queueServiceClient = QueueServiceClient.fromConnectionString(connectionString);
    const queueClient = queueServiceClient.getQueueClient(queueName);

    console.log(`Connecting to queue: ${queueName}`);
    
    const exists = await queueClient.exists();
    if (!exists) {
        console.log(`Queue ${queueName} does not exist. Creating queue...`);
        await queueClient.create();
    }

    console.log('Polling for messages...\n');

    while (true) {
        const response = await queueClient.receiveMessages({
            numberOfMessages: 10,
            visibilityTimeout: 60
        });

        if (response.receivedMessageItems.length === 0) {
            console.log('No messages in queue. Waiting 5 seconds...');
            await new Promise(resolve => setTimeout(resolve, 5000));
            continue;
        }

        for (const message of response.receivedMessageItems) {
            console.log('=====================================');
            console.log('New message received:');
            console.log('Message ID:', message.messageId);
            console.log('Insert Time:', message.insertedOn);
            console.log('Dequeue Count:', message.dequeueCount);
            
            let messageContent;
            try {
                const decodedText = Buffer.from(message.messageText, 'base64').toString('utf-8');
                messageContent = JSON.parse(decodedText);
                console.log('Message Content (parsed):');
                console.log(JSON.stringify(messageContent, null, 2));
            } catch (e) {
                console.log('Message Content (raw):', message.messageText);
                messageContent = message.messageText;
            }

            if (messageContent && typeof messageContent === 'object') {
                await handleStorageEvent(messageContent);
            }

            console.log('Deleting message from queue...');
            await queueClient.deleteMessage(message.messageId, message.popReceipt);
            console.log('Message processed and deleted.\n');
        }
    }
}

async function handleStorageEvent(eventData) {
    if (eventData.eventType && eventData.eventType.includes('Blob')) {
        console.log('\n--- Storage Blob Event Detected ---');
        console.log('Event Type:', eventData.eventType);
        console.log('Event Time:', eventData.eventTime);
        
        if (eventData.data && eventData.data.url) {
            const blobUrl = eventData.data.url;
            console.log('Blob URL:', blobUrl);
            
            // Parse URL: https://accountname.blob.core.windows.net/container/path/to/blob
            const url = new URL(blobUrl);
            const pathParts = url.pathname.substring(1).split('/'); // Remove leading '/' and split
            const detectedContainer = pathParts[0];
            const blobName = pathParts.slice(1).join('/');
            
            console.log('Container:', detectedContainer);
            console.log('Blob Name:', blobName);
            
            if (eventData.eventType === 'Microsoft.Storage.BlobCreated' || 
                eventData.eventType === 'Microsoft.Storage.BlobModified') {
                await downloadBlob(detectedContainer || containerName, blobName);
            }
        }
    } else if (eventData.topic && eventData.subject) {
        console.log('\n--- Event Grid Event Detected ---');
        console.log('Topic:', eventData.topic);
        console.log('Subject:', eventData.subject);
        console.log('Event Type:', eventData.eventType);
        
        if (eventData.subject.includes('/blobs/')) {
            const subjectParts = eventData.subject.split('/');
            const containerIdx = subjectParts.indexOf('containers');
            const blobsIdx = subjectParts.indexOf('blobs');
            
            if (containerIdx !== -1 && blobsIdx !== -1) {
                const detectedContainer = subjectParts[containerIdx + 1];
                const blobName = subjectParts.slice(blobsIdx + 1).join('/');
                
                console.log('Container:', detectedContainer);
                console.log('Blob Name:', blobName);
                
                if (eventData.eventType === 'Microsoft.Storage.BlobCreated' || 
                    eventData.eventType === 'Microsoft.Storage.BlobModified') {
                    await downloadBlob(detectedContainer, blobName);
                }
            }
        }
    } else {
        console.log('\n--- Custom Event Format ---');
        if (eventData.blobName || eventData.fileName) {
            const blobName = eventData.blobName || eventData.fileName;
            const container = eventData.container || containerName;
            console.log('Container:', container);
            console.log('Blob Name:', blobName);
            await downloadBlob(container, blobName);
        }
    }
}

async function downloadBlob(containerName, blobName) {
    if (!containerName || !blobName) {
        console.log('Container name or blob name not specified. Skipping download.');
        return;
    }

    console.log(`\nDownloading blob: ${blobName} from container: ${containerName}`);
    
    try {
        const blobServiceClient = BlobServiceClient.fromConnectionString(connectionString);
        const containerClient = blobServiceClient.getContainerClient(containerName);
        const blobClient = containerClient.getBlobClient(blobName);
        
        const exists = await blobClient.exists();
        if (!exists) {
            console.log(`Blob ${blobName} does not exist in container ${containerName}`);
            return;
        }

        const properties = await blobClient.getProperties();
        console.log(`Blob size: ${properties.contentLength} bytes`);
        console.log(`Content type: ${properties.contentType}`);
        console.log(`Last modified: ${properties.lastModified}`);
        
        const downloadDir = './downloads';
        await fs.mkdir(downloadDir, { recursive: true });
        
        const fileName = path.basename(blobName);
        const downloadPath = path.join(downloadDir, fileName);
        
        console.log(`Downloading to: ${downloadPath}`);
        const downloadResponse = await blobClient.downloadToFile(downloadPath);
        
        console.log(`✓ Successfully downloaded: ${downloadPath}`);
        console.log(`  Downloaded ${downloadResponse.contentLength} bytes`);
        
    } catch (error) {
        console.error('Error downloading blob:', error.message);
    }
}

async function main() {
    console.log('Azure Storage Queue Event Processor');
    console.log('====================================\n');
    
    try {
        await processQueueMessages();
    } catch (error) {
        console.error('Fatal error:', error);
        process.exit(1);
    }
}

main();