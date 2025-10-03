require('dotenv').config();

const { firecrawlRequest } = require('./firecrawlRequest');

function markdownToPlainText(markdown) {
  if (typeof markdown !== 'string') {
    return '';
  }

  return markdown
    .replace(/```[\s\S]*?```/g, '')
    .replace(/\!\[[^\]]*\]\([^\)]+\)/g, '')
    .replace(/\[(.*?)\]\((.*?)\)/g, '$1 ($2)')
    .replace(/^\s*#+\s*/gm, '')
    .replace(/^>\s?/gm, '')
    .replace(/[\*_]{1,3}([^\*_]+)[\*_]{1,3}/g, '$1')
    .trim();
}

const args = process.argv.slice(2);
const jsonFlagIndex = args.indexOf('--json');
const outputJson = jsonFlagIndex !== -1;

if (outputJson) {
  args.splice(jsonFlagIndex, 1);
}

const targetUrl = args[0];

if (!targetUrl) {
  console.error('使い方: node scrape.js <URL> [--json]');
  process.exit(1);
}

const apiKey = process.env.FIRECRAWL_API_KEY;

if (!apiKey) {
  console.error('FIRECRAWL_API_KEY が .env に設定されていません');
  process.exit(1);
}

firecrawlRequest(apiKey, '/v2/scrape', { url: targetUrl })
  .then((result) => {
    if (!result) {
      console.log('');
      return;
    }

    if (outputJson) {
      console.log(JSON.stringify(result, null, 2));
      return;
    }

    const payload = result && typeof result === 'object' && 'data' in result ? result.data : result;

    const plainText = typeof payload.text === 'string' ? payload.text.trim() : markdownToPlainText(payload.markdown);
    const fallback = typeof payload.content === 'string' ? payload.content.trim() : '';
    const markdown = typeof payload.markdown === 'string' ? payload.markdown.trim() : '';
    const output = plainText || fallback || markdown || JSON.stringify(result, null, 2);

    console.log(output);
  })
  .catch((error) => {
    console.error(error.message);
    process.exit(1);
  });
