const https = require('https');

const API_HOST = 'api.firecrawl.dev';

function firecrawlRequest(apiKey, path, payload) {
  const data = JSON.stringify(payload);

  return new Promise((resolve, reject) => {
    const request = https.request(
      {
        hostname: API_HOST,
        path,
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
          'Content-Length': Buffer.byteLength(data),
          Authorization: `Bearer ${apiKey}`,
        },
      },
      (response) => {
        let body = '';

        response.on('data', (chunk) => {
          body += chunk;
        });

        response.on('end', () => {
          const trimmed = body.trim();
          const parsed = trimmed ? JSON.parse(trimmed) : null;

          if (response.statusCode && response.statusCode >= 400) {
            const message = parsed && parsed.message ? parsed.message : trimmed || response.statusMessage;
            reject(new Error(`Firecrawl API error (${response.statusCode}): ${message}`));
            return;
          }

          resolve(parsed);
        });
      },
    );

    request.on('error', reject);
    request.write(data);
    request.end();
  });
}

module.exports = {
  firecrawlRequest,
};
