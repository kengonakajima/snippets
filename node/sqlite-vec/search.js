import { searchSimilar } from './db.js';
import { getEmbedding } from './embeddings.js';
import chalk from 'chalk';
import Table from 'cli-table3';

export async function performSearch(query, limit = 5) {
  console.log(chalk.blue(`Searching for: "${query}"`));
  
  const queryEmbedding = await getEmbedding(query);
  const results = searchSimilar(queryEmbedding, limit);

  if (results.length === 0) {
    console.log(chalk.yellow('No results found.'));
    return [];
  }

  const table = new Table({
    head: [
      chalk.cyan('Score'),
      chalk.cyan('ID'),
      chalk.cyan('Text'),
      chalk.cyan('Created')
    ],
    colWidths: [12, 8, 60, 22],
    wordWrap: true
  });

  results.forEach(result => {
    const score = (1 - result.distance).toFixed(4);
    const textPreview = result.text.length > 100 
      ? result.text.substring(0, 97) + '...' 
      : result.text;
    
    table.push([
      chalk.green(score),
      result.id,
      textPreview,
      new Date(result.created_at).toLocaleString()
    ]);
  });

  console.log(table.toString());
  return results;
}