import express from 'express';
import { fileURLToPath } from 'url';
import { dirname, join } from 'path';
import { promises as fs } from 'fs';
import open from 'open';
import cors from 'cors';

const __filename = fileURLToPath(import.meta.url);
const __dirname = dirname(__filename);

const app = express();
const PORT = process.env.PORT || 3000;
const DATA_DIR = join(__dirname, 'data');

// Enable CORS for all routes
app.use(cors());

// Parse JSON bodies
app.use(express.json());

// Serve static files from the current directory
app.use(express.static(__dirname));

// Initialize data directory
async function initializeDataDirectory() {
  try {
    await fs.mkdir(DATA_DIR, { recursive: true });
    console.log('📁 Data directory initialized');
  } catch (error) {
    console.error('Failed to create data directory:', error.message);
  }
}

// API endpoint for saving data to JSON files
app.post('/api/save-data', async (req, res) => {
  try {
    const { type, data, timestamp } = req.body;
    
    if (!type || !data) {
      return res.status(400).json({ error: 'Type and data are required' });
    }
    
    const filename = `${type}.json`;
    const filepath = join(DATA_DIR, filename);
    
    const saveData = {
      data: data,
      lastUpdated: timestamp || new Date().toISOString(),
      version: '1.0'
    };
    
    await fs.writeFile(filepath, JSON.stringify(saveData, null, 2), 'utf8');
    
    res.json({ 
      success: true, 
      message: `Data saved to ${filename}`,
      timestamp: saveData.lastUpdated
    });
    
  } catch (error) {
    console.error('Failed to save data:', error);
    res.status(500).json({ 
      error: 'Failed to save data',
      message: error.message 
    });
  }
});

// API endpoint for loading data from JSON files
app.get('/api/load-data/:type', async (req, res) => {
  try {
    const { type } = req.params;
    const filename = `${type}.json`;
    const filepath = join(DATA_DIR, filename);
    
    try {
      const fileContent = await fs.readFile(filepath, 'utf8');
      const savedData = JSON.parse(fileContent);
      
      res.json({
        success: true,
        data: savedData.data,
        lastUpdated: savedData.lastUpdated,
        version: savedData.version
      });
    } catch (fileError) {
      // File doesn't exist or is corrupted
      res.status(404).json({
        success: false,
        error: 'Data file not found',
        message: `No saved data found for type: ${type}`
      });
    }
    
  } catch (error) {
    console.error('Failed to load data:', error);
    res.status(500).json({ 
      error: 'Failed to load data',
      message: error.message 
    });
  }
});

// API endpoint for getting all available data types
app.get('/api/data-types', async (req, res) => {
  try {
    const files = await fs.readdir(DATA_DIR);
    const dataTypes = files
      .filter(file => file.endsWith('.json'))
      .map(file => file.replace('.json', ''));
    
    res.json({
      success: true,
      dataTypes: dataTypes,
      count: dataTypes.length
    });
  } catch (error) {
    console.error('Failed to get data types:', error);
    res.status(500).json({ 
      error: 'Failed to get data types',
      message: error.message 
    });
  }
});

// Serve index.html at the root route
app.get('/', (req, res) => {
  res.sendFile(join(__dirname, 'index.html'));
});

// Start server function
async function startServer() {
  try {
    // Initialize data directory before starting server
    await initializeDataDirectory();
    
    const server = app.listen(PORT, () => {
      const url = `http://localhost:${PORT}`;
      console.log(`🚀 Project Timeline App server running at ${url}`);
      
      // Auto-open browser
      open(url).catch(err => {
        console.log('Could not auto-open browser:', err.message);
        console.log(`Please manually open: ${url}`);
      });
    });

    // Handle server shutdown gracefully
    process.on('SIGINT', () => {
      console.log('\n🛑 Shutting down server...');
      server.close(() => {
        console.log('✅ Server stopped successfully');
        process.exit(0);
      });
    });

    return server;
  } catch (error) {
    console.error('❌ Failed to start server:', error.message);
    process.exit(1);
  }
}

// Start the server
startServer();