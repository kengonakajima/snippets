#!/usr/bin/env node
const fs = require('fs');
const fsp = require('fs/promises');
const path = require('path');
const { performance } = require('perf_hooks');
const { Pool } = require('pg');
const crypto = require('crypto');
const dotenv = require('dotenv');
const yargs = require('yargs/yargs');
const { hideBin } = require('yargs/helpers');

dotenv.config();
const numEnv = (name, fallback) => {
  const raw = process.env[name];
  if (!raw) return fallback;
  const value = Number(raw);
  if (Number.isNaN(value)) throw new Error(`環境変数${name}は数値で指定してください (現在: ${raw})`);
  return value;
};
const CONFIG = {
  url: process.env.DATABASE_URL || process.env.PG_CONNECTION_STRING || 'postgres://localhost:5432/postgres',
  commonKey: process.env.COMMON_KEY,
  userBaseKey: process.env.USER_BASE_KEY,
  pbkdf2Iterations: numEnv('PBKDF2_ITERATIONS', 100000),
  pbkdf2KeyLength: numEnv('PBKDF2_KEY_LENGTH', 32),
  pbkdf2Digest: process.env.PBKDF2_DIGEST || 'sha256',
  requests: numEnv('LIKE_SELECT_REQUESTS', 300),
  resultsDir: process.env.RESULTS_DIR || 'results'
};
const argv = yargs(hideBin(process.argv))
  .option('pattern', { alias: 'p', type: 'number', choices: [1, 2, 3], demandOption: true, describe: '暗号化パターン' })
  .option('requests', { type: 'number', default: CONFIG.requests, describe: '計測リクエスト数' })
  .option('warmup', { type: 'number', default: 1 })
  .option('runs', { type: 'number', default: 3 })
  .help()
  .strict()
  .parseSync();
const ensureKeys = (pattern) => {
  if (pattern >= 2 && !CONFIG.commonKey) throw new Error('COMMON_KEY が設定されていません');
  if (pattern === 3 && !CONFIG.userBaseKey) throw new Error('USER_BASE_KEY が設定されていません');
};
const ensureResultsDir = () => {
  const dir = path.resolve(process.cwd(), CONFIG.resultsDir);
  if (!fs.existsSync(dir)) fs.mkdirSync(dir, { recursive: true });
  return dir;
};
const userKeyCache = new Map();
const deriveUserKey = (userId) => {
  if (userKeyCache.has(userId)) return userKeyCache.get(userId);
  const key = crypto
    .pbkdf2Sync(CONFIG.userBaseKey, userId, CONFIG.pbkdf2Iterations, CONFIG.pbkdf2KeyLength, CONFIG.pbkdf2Digest)
    .toString('hex');
  userKeyCache.set(userId, key);
  return key;
};
const mean = (arr) => (arr.length ? arr.reduce((sum, v) => sum + v, 0) / arr.length : 0);
const median = (arr) => {
  if (!arr.length) return 0;
  const sorted = [...arr].sort((a, b) => a - b);
  const mid = Math.floor(sorted.length / 2);
  return sorted.length % 2 ? sorted[mid] : (sorted[mid - 1] + sorted[mid]) / 2;
};
const stddev = (arr) => {
  if (arr.length <= 1) return 0;
  const avg = mean(arr);
  return Math.sqrt(arr.reduce((sum, v) => sum + (v - avg) ** 2, 0) / (arr.length - 1));
};
const writeJsonResult = async (pattern, durations, latencyStats, hitStats, throughput, params) => {
  const dir = ensureResultsDir();
  const timestamp = new Date().toISOString().replace(/[:.]/g, '-');
  const payload = {
    scenario: 'like-search',
    pattern,
    recordedAt: new Date().toISOString(),
    parameters: params,
    durationsMs: durations,
    perRequestLatencyMs: latencyStats,
    hitCountStats: hitStats,
    throughputReqPerSec: throughput,
    stats: { meanMs: mean(durations), medianMs: median(durations), stddevMs: stddev(durations) }
  };
  const file = path.join(dir, `${timestamp}-like-search-p${pattern}.json`);
  await fsp.writeFile(file, JSON.stringify(payload, null, 2));
  return { file, payload };
};
const appendSummaryRows = async (rows) => {
  if (!rows.length) return;
  const dir = ensureResultsDir();
  const csv = path.join(dir, 'summary.csv');
  try { await fsp.access(csv); } catch (_) { await fsp.writeFile(csv, 'timestamp,scenario,pattern,metric,value,unit\n'); }
  const timestamp = new Date().toISOString();
  const lines = rows.map(({ scenario, pattern, metric, value, unit = '' }) => [timestamp, scenario, `p${pattern}`, metric, value, unit]
    .map((cell) => (typeof cell === 'string' && cell.includes(',') ? `"${cell}"` : cell)).join(','));
  await fsp.appendFile(csv, `${lines.join('\n')}\n`);
};
const pick = (arr) => arr[Math.floor(Math.random() * arr.length)];
const tokenize = (message) => {
  const words = message.split(/\s+/).filter((w) => w.length >= 4);
  const base = words.length ? pick(words) : message;
  if (!base) return null;
  const start = Math.floor(Math.max(1, base.length - 4) * Math.random());
  const len = Math.min(5, base.length - start);
  return base.slice(start, start + len) || base;
};
const collectTokens = async (pool, pattern, limit = 500) => {
  if (pattern === 1) {
    const { rows } = await pool.query("SELECT convert_from(message, 'utf8') AS message FROM posts LIMIT $1", [limit]);
    return rows.map((row) => tokenize(row.message)).filter(Boolean);
  }
  if (pattern === 2) {
    const { rows } = await pool.query('SELECT pgp_sym_decrypt(message, $2)::text AS message FROM posts LIMIT $1', [limit, CONFIG.commonKey]);
    return rows.map((row) => tokenize(row.message)).filter(Boolean);
  }
  const tokens = [];
  const { rows: users } = await pool.query('SELECT DISTINCT created_by FROM posts LIMIT $1', [Math.min(limit, 200)]);
  for (const { created_by: userId } of users) {
    const key = deriveUserKey(userId);
    const { rows } = await pool.query('SELECT pgp_sym_decrypt(message, $2)::text AS message FROM posts WHERE created_by = $1 LIMIT 20', [userId, key]);
    rows.forEach((row) => {
      const term = tokenize(row.message);
      if (term) tokens.push({ userId, key, term });
    });
  }
  return tokens;
};
const runBenchmark = async (task, warmup, runs) => {
  for (let i = 0; i < warmup; i += 1) await task();
  const durations = [];
  const results = [];
  for (let i = 0; i < runs; i += 1) {
    const start = performance.now();
    const result = await task();
    durations.push(performance.now() - start);
    results.push(result);
  }
  return { durations, results };
};
async function main() {
  ensureKeys(argv.pattern);
  const pool = new Pool({ connectionString: CONFIG.url });
  const tokens = await collectTokens(pool, argv.pattern);
  if (!tokens.length) {
    throw new Error('LIKE検索用トークンを抽出できませんでした。seedが完了しているか確認してください。');
  }
  const task = async () => {
    const latencies = [];
    const hits = [];
    const client = await pool.connect();
    try {
      for (let i = 0; i < argv.requests; i += 1) {
        const start = performance.now();
        let count = 0;
        if (argv.pattern === 1) {
          const term = pick(tokens);
          const { rows } = await client.query("SELECT count(*)::int FROM posts WHERE convert_from(message, 'utf8') ILIKE $1", [`%${term}%`]);
          count = rows[0].count;
        } else if (argv.pattern === 2) {
          const term = pick(tokens);
          const { rows } = await client.query(
            'SELECT count(*)::int FROM posts WHERE pgp_sym_decrypt(message, $2)::text ILIKE $1',
            [`%${term}%`, CONFIG.commonKey]
          );
          count = rows[0].count;
        } else {
          const sample = pick(tokens);
          const { rows } = await client.query('SELECT count(*)::int FROM posts WHERE created_by = $2 AND pgp_sym_decrypt(message, $3)::text ILIKE $1', [`%${sample.term}%`, sample.userId, sample.key]);
          count = rows[0].count;
        }
        const end = performance.now();
        latencies.push(end - start);
        hits.push(count);
      }
    } finally {
      client.release();
    }
    return { latencies, hits };
  };
  const { durations, results } = await runBenchmark(task, argv.warmup, argv.runs);
  const allLatencies = results.flatMap((r) => r.latencies);
  const allHits = results.flatMap((r) => r.hits);
  const latencyStats = { meanMs: mean(allLatencies), medianMs: median(allLatencies), stddevMs: stddev(allLatencies) };
  const hitStats = { mean: mean(allHits), median: median(allHits), stddev: stddev(allHits) };
  const throughput = durations.map((d) => argv.requests / (d / 1000));
  await writeJsonResult(argv.pattern, durations, latencyStats, hitStats, throughput, {
    requests: argv.requests,
    warmup: argv.warmup,
    runs: argv.runs
  });
  await appendSummaryRows([
    { scenario: 'like-search', pattern: argv.pattern, metric: 'median_ms', value: latencyStats.medianMs.toFixed(4) },
    { scenario: 'like-search', pattern: argv.pattern, metric: 'mean_ms', value: latencyStats.meanMs.toFixed(4) },
    { scenario: 'like-search', pattern: argv.pattern, metric: 'stddev_ms', value: latencyStats.stddevMs.toFixed(4) },
    { scenario: 'like-search', pattern: argv.pattern, metric: 'median_req_per_sec', value: median(throughput).toFixed(2), unit: 'req/s' }
  ]);
  await pool.end();
  console.log(`LIKE検索ベンチ完了: パターン${argv.pattern}`);
}
main().catch((error) => { console.error('LIKE検索ベンチでエラーが発生しました:', error); process.exit(1); });
