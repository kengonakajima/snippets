require('dotenv').config();

const fs = require('fs');
const path = require('path');
const http = require('http');
const https = require('https');
const { firecrawlRequest } = require('./firecrawlRequest');

const fsPromises = fs.promises;

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

function ensureDir(dirPath) {
  return fsPromises.mkdir(dirPath, { recursive: true });
}

function downloadImage(url, destination, redirectCount = 0) {
  if (redirectCount > 5) {
    return Promise.reject(new Error('リダイレクト回数が多すぎます'));
  }

  const protocol = url.startsWith('https') ? https : http;

  return new Promise((resolve, reject) => {
    const request = protocol.get(url, (response) => {
      if (response.statusCode && response.statusCode >= 300 && response.statusCode < 400 && response.headers.location) {
        response.resume();
        const redirectTarget = response.headers.location.startsWith('http')
          ? response.headers.location
          : new URL(response.headers.location, url).toString();

        downloadImage(redirectTarget, destination, redirectCount + 1)
          .then(resolve)
          .catch(reject);
        return;
      }

      if (response.statusCode !== 200) {
        response.resume();
        reject(new Error(`画像の取得に失敗しました (status: ${response.statusCode})`));
        return;
      }

      const fileStream = fs.createWriteStream(destination);

      response.pipe(fileStream);

      fileStream.on('finish', () => {
        fileStream.close(resolve);
      });

      fileStream.on('error', (streamError) => {
        fsPromises.unlink(destination)
          .catch(() => {})
          .finally(() => {
            reject(streamError);
          });
      });
    });

    request.on('error', reject);
  });
}

function resolveImageUrl(sourceUrl, baseUrl) {
  if (!sourceUrl) {
    return '';
  }

  const trimmed = sourceUrl.trim().replace(/^<|>$/g, '');

  if (/^https?:\/\//i.test(trimmed)) {
    return trimmed;
  }

  if (!baseUrl || !/^https?:\/\//i.test(baseUrl)) {
    return '';
  }

  const baseParsed = new URL(baseUrl);
  const resolvedParsed = new URL(trimmed, baseParsed);

  if (!/^https?:$/i.test(resolvedParsed.protocol)) {
    return '';
  }

  return resolvedParsed.toString();
}

async function saveMarkdownWithImages(markdownContent, baseUrl, markdownOutPath) {
  const content = typeof markdownContent === 'string' ? markdownContent : '';

  if (!content) {
    await ensureDir(path.dirname(markdownOutPath));
    await fsPromises.writeFile(markdownOutPath, '', 'utf8');
    return { saved: true, imagesSaved: 0 };
  }

  const outputDir = path.dirname(markdownOutPath);
  const imagesDir = path.join(outputDir, 'images');

  await ensureDir(outputDir);
  await ensureDir(imagesDir);

  const imagePattern = /!\[([^\]]*)\]\(([^)]+)\)/g;
  let updatedMarkdown = content;
  let match;
  let index = 1;

  while ((match = imagePattern.exec(content)) !== null) {
    const fullMatch = match[0];
    const altText = match[1];
    const targetWithMeta = match[2].trim();
    const titleMatch = targetWithMeta.match(/\s+"([^"]*)"\s*$/);

    let imageUrlText = targetWithMeta;
    let titleText = '';

    if (titleMatch && typeof titleMatch.index === 'number') {
      imageUrlText = targetWithMeta.slice(0, titleMatch.index).trim();
      titleText = titleMatch[1];
    }

    if (!imageUrlText) {
      continue;
    }

    const resolved = resolveImageUrl(imageUrlText, baseUrl);

    if (!resolved || !/^https?:\/\//i.test(resolved)) {
      continue;
    }

    const cleanWithoutHash = resolved.split('#')[0];
    const cleanWithoutQuery = cleanWithoutHash.split('?')[0];
    let extension = path.extname(cleanWithoutQuery).toLowerCase();

    if (!extension || extension === '.') {
      extension = '.png';
    }

    const fileName = `img-${String(index).padStart(3, '0')}${extension}`;
    const destinationPath = path.join(imagesDir, fileName);
    const downloadSucceeded = await downloadImage(resolved, destinationPath)
      .then(() => true)
      .catch(() => false);

    if (!downloadSucceeded) {
      continue;
    }

    index += 1;

    const relativePath = `images/${fileName}`;
    const titleSuffix = titleText ? ` "${titleText}"` : '';
    const replacement = `![${altText}](${relativePath}${titleSuffix})`;

    updatedMarkdown = updatedMarkdown.replace(fullMatch, replacement);
  }

  await fsPromises.writeFile(markdownOutPath, updatedMarkdown, 'utf8');

  return { saved: true, imagesSaved: index - 1 };
}

const args = process.argv.slice(2);
const jsonFlagIndex = args.indexOf('--json');
const outputJson = jsonFlagIndex !== -1;

if (outputJson) {
  args.splice(jsonFlagIndex, 1);
}

const markdownOutIndex = args.indexOf('--markdown-out');
let markdownOutPath = '';

if (markdownOutIndex !== -1) {
  const outputPathCandidate = args[markdownOutIndex + 1];

  if (!outputPathCandidate || outputPathCandidate.startsWith('--')) {
    console.error('--markdown-out の次には出力ファイルパスを指定してください');
    process.exit(1);
  }

  markdownOutPath = outputPathCandidate;
  args.splice(markdownOutIndex, 2);
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
  .then(async (result) => {
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

    if (markdownOutPath) {
      const markdownContent = markdown || output;
      const saveResult = await saveMarkdownWithImages(markdownContent, targetUrl, markdownOutPath);

      if (saveResult && saveResult.saved) {
        console.log(`Markdown を保存しました: ${markdownOutPath}`);
      }
    }

    console.log(output);
  })
  .catch((error) => {
    console.error(error.message);
    process.exit(1);
  });
