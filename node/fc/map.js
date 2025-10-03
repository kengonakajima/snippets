require('dotenv').config();

const { firecrawlRequest } = require('./firecrawlRequest');

function createNode() {
  return { children: new Map(), terminal: false };
}

function insertIntoTree(tree, origin, segments) {
  if (!tree.has(origin)) {
    tree.set(origin, createNode());
  }

  let current = tree.get(origin);

  segments.forEach((segment, index) => {
    if (!current.children.has(segment)) {
      current.children.set(segment, createNode());
    }

    current = current.children.get(segment);

    if (index === segments.length - 1) {
      current.terminal = true;
    }
  });
}

function createSegments(urlObject) {
  const segments = urlObject.pathname.split('/').filter(Boolean);

  if (segments.length === 0) {
    segments.push('/');
  }

  if (urlObject.pathname.endsWith('/') && segments[segments.length - 1] !== '/') {
    segments[segments.length - 1] = `${segments[segments.length - 1]}/`;
  }

  const suffix = `${urlObject.search}${urlObject.hash}`;

  if (suffix) {
    const lastIndex = segments.length - 1;
    segments[lastIndex] = `${segments[lastIndex]}${suffix}`;
  }

  return segments;
}

function buildTree(urls) {
  const uniqueUrls = Array.from(new Set(urls));
  const tree = new Map();

  uniqueUrls.forEach((urlString) => {
    try {
      const urlObject = new URL(urlString);
      const origin = `${urlObject.protocol}//${urlObject.host}`;
      const segments = createSegments(urlObject);

      insertIntoTree(tree, origin, segments);
    } catch (error) {
      // URL として処理できない場合は無視
    }
  });

  return tree;
}

function printTree(tree) {
  const origins = Array.from(tree.keys()).sort((a, b) => a.localeCompare(b));

  if (origins.length === 0) {
    console.log('リンクが見つかりませんでした。');
    return;
  }

  origins.forEach((origin) => {
    console.log(origin);
    const node = tree.get(origin);
    printChildren(node.children, '');
  });
}

function printChildren(childrenMap, prefix) {
  const entries = Array.from(childrenMap.entries()).sort((a, b) => a[0].localeCompare(b[0]));

  entries.forEach(([label, child], index) => {
    const isLast = index === entries.length - 1;
    const connector = isLast ? '└── ' : '├── ';
    console.log(`${prefix}${connector}${label}`);

    if (child.children.size > 0) {
      const nextPrefix = `${prefix}${isLast ? '    ' : '│   '}`;
      printChildren(child.children, nextPrefix);
    }
  });
}

function extractLinks(result) {
  if (result && typeof result === 'object') {
    if (Array.isArray(result.links)) {
      return result.links;
    }

    if (result.data && typeof result.data === 'object' && Array.isArray(result.data.links)) {
      return result.data.links;
    }
  }

  return [];
}

const args = process.argv.slice(2);
const jsonFlagIndex = args.indexOf('--json');
const outputJson = jsonFlagIndex !== -1;

if (outputJson) {
  args.splice(jsonFlagIndex, 1);
}

const targetUrl = args[0];

if (!targetUrl) {
  console.error('使い方: node map.js <URL> [--json]');
  process.exit(1);
}

const apiKey = process.env.FIRECRAWL_API_KEY;

if (!apiKey) {
  console.error('FIRECRAWL_API_KEY が .env に設定されていません');
  process.exit(1);
}

firecrawlRequest(apiKey, '/v2/map', { url: targetUrl })
  .then((result) => {
    if (outputJson) {
      console.log(JSON.stringify(result, null, 2));
      return;
    }

    const linkEntries = extractLinks(result);
    const urls = linkEntries
      .map((entry) => {
        if (typeof entry === 'string') {
          return entry;
        }

        if (entry && typeof entry === 'object' && typeof entry.url === 'string') {
          return entry.url;
        }

        return null;
      })
      .filter(Boolean);

    const tree = buildTree(urls);
    printTree(tree);
  })
  .catch((error) => {
    console.error(error.message);
    process.exit(1);
  });
