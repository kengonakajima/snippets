require('dotenv').config();

const { firecrawlRequest } = require('./firecrawlRequest');

function escapeRegExp(text) {
  return text.replace(/[-/\\^$*+?.()|[\]{}]/g, '\\$&');
}

function highlightText(text, terms) {
  if (typeof text !== 'string' || terms.length === 0) {
    return text || '';
  }

  return terms
    .map((term) => term.trim())
    .filter(Boolean)
    .reduce((acc, term) => {
      const escaped = escapeRegExp(term);
      if (!escaped) {
        return acc;
      }

      const pattern = new RegExp(
        `(?<![\\p{L}\\p{N}_])(${escaped})(?![\\p{L}\\p{N}_])`,
        'giu',
      );

      return acc.replace(pattern, '\u001b[43m\u001b[30m$1\u001b[0m');
    }, text);
}

function extractResults(result) {
  if (result && typeof result === 'object') {
    if (Array.isArray(result.web)) {
      return result.web;
    }

    if (result.data && typeof result.data === 'object' && Array.isArray(result.data.web)) {
      return result.data.web;
    }

    if (result.data && Array.isArray(result.data)) {
      return result.data;
    }
  }

  return [];
}

function combinedText(entry) {
  if (entry && typeof entry === 'object') {
    return [entry.url, entry.title, entry.description]
      .filter((value) => typeof value === 'string')
      .join(' ');
  }

  return typeof entry === 'string' ? entry : '';
}

const args = process.argv.slice(2);
const jsonFlagIndex = args.indexOf('--json');
const outputJson = jsonFlagIndex !== -1;

if (outputJson) {
  args.splice(jsonFlagIndex, 1);
}

const query = args.join(' ').trim();

if (!query) {
  console.error('使い方: node search.js <キーワード> [--json]');
  process.exit(1);
}

const apiKey = process.env.FIRECRAWL_API_KEY;

if (!apiKey) {
  console.error('FIRECRAWL_API_KEY が .env に設定されていません');
  process.exit(1);
}

const responseTerms = query.split(/\s+/).filter(Boolean);
const lowercaseTerms = responseTerms.map((term) => term.toLowerCase());

firecrawlRequest(apiKey, '/v2/search', { query })
  .then((result) => {
    if (outputJson) {
      console.log(JSON.stringify(result, null, 2));
      return;
    }

    const results = extractResults(result);

    if (results.length === 0) {
      console.log('結果が見つかりませんでした。');
      return;
    }

    const filtered = results.filter((entry) => {
      if (lowercaseTerms.length === 0) {
        return true;
      }

      const text = combinedText(entry).toLowerCase();

      return lowercaseTerms.every((term) => text.includes(term));
    });

    if (filtered.length === 0) {
      console.log('検索語に完全一致する結果が見つかりませんでした。');
      return;
    }

    filtered.forEach((entry, index) => {
      const url = entry && typeof entry === 'object' ? entry.url : entry;
      const description = entry && typeof entry === 'object' ? entry.description || '' : '';
      const fallback = entry && typeof entry === 'object' ? entry.title || '' : '';
      const snippet = description || fallback || '(スニペットなし)';

      console.log(url || '(URLなし)');
      console.log(highlightText(snippet, responseTerms));

      if (index !== filtered.length - 1) {
        console.log('');
      }
    });
  })
  .catch((error) => {
    console.error(error.message);
    process.exit(1);
  });
