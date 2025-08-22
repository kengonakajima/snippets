#!/usr/bin/env node

import { Command } from 'commander';
import chalk from 'chalk';
import Table from 'cli-table3';
import { 
  initDatabase, 
  insertDocument,
  insertDocumentsBatch,
  listDocuments, 
  deleteDocument, 
  clearAllDocuments 
} from './db.js';
import { getEmbedding } from './embeddings.js';
import { performSearch } from './search.js';
import { loadSampleData } from './sample-data.js';

const program = new Command();

initDatabase();

program
  .name('sqlite-vec-demo')
  .description('Vector search demo using sqlite-vec and OpenAI embeddings')
  .version('1.0.0');

program
  .command('add <text>')
  .description('Add a new document to the database')
  .action(async (text) => {
    console.log(chalk.blue('Adding document...'));
    const embedding = await getEmbedding(text);
    const id = insertDocument(text, embedding);
    console.log(chalk.green(`✓ Document added with ID: ${id}`));
  });

program
  .command('search <query>')
  .description('Search for similar documents')
  .option('-l, --limit <number>', 'Number of results to return', '5')
  .action(async (query, options) => {
    const limit = parseInt(options.limit);
    await performSearch(query, limit);
  });

program
  .command('list')
  .description('List all documents')
  .action(() => {
    const documents = listDocuments();
    
    if (documents.length === 0) {
      console.log(chalk.yellow('No documents found.'));
      return;
    }

    const table = new Table({
      head: [chalk.cyan('ID'), chalk.cyan('Text'), chalk.cyan('Created')],
      colWidths: [8, 70, 22],
      wordWrap: true
    });

    documents.forEach(doc => {
      const textPreview = doc.text.length > 100 
        ? doc.text.substring(0, 97) + '...' 
        : doc.text;
      
      table.push([
        doc.id,
        textPreview,
        new Date(doc.created_at).toLocaleString()
      ]);
    });

    console.log(table.toString());
    console.log(chalk.blue(`Total documents: ${documents.length}`));
  });

program
  .command('delete <id>')
  .description('Delete a document by ID')
  .action((id) => {
    const success = deleteDocument(parseInt(id));
    if (success) {
      console.log(chalk.green(`✓ Document ${id} deleted successfully`));
    } else {
      console.log(chalk.red(`✗ Document ${id} not found`));
    }
  });

program
  .command('clear')
  .description('Clear all documents from the database')
  .action(() => {
    clearAllDocuments();
    console.log(chalk.green('✓ All documents cleared'));
  });

program
  .command('load-sample')
  .description('Load sample data into database')
  .action(async () => {
    await loadSampleData();
  });

program.parseAsync(process.argv).catch((error) => {
  console.error(chalk.red('Error:'), error.message);
  process.exit(1);
});