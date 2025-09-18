#!/usr/bin/env node
const fs = require('fs');
const fsp = require('fs/promises');
const path = require('path');
const { performance } = require('perf_hooks');
const { Pool } = require('pg');
const { ulid } = require('ulid');
const { faker } = require('@faker-js/faker');
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
  totalRows: numEnv('BENCH_TOTAL_ROWS', 10000),
  userCount: numEnv('BENCH_USER_COUNT', 1000),
  batchSize: numEnv('BENCH_BATCH_SIZE', 500),
  commonKey: process.env.COMMON_KEY,
  userBaseKey: process.env.USER_BASE_KEY,
  pbkdf2Iterations: numEnv('PBKDF2_ITERATIONS', 100000),
  pbkdf2KeyLength: numEnv('PBKDF2_KEY_LENGTH', 32),
  pbkdf2Digest: process.env.PBKDF2_DIGEST || 'sha256',
  resultsDir: process.env.RESULTS_DIR || 'results'
};
const argv = yargs(hideBin(process.argv))
  .option('pattern', { alias: 'p', type: 'number', choices: [1, 2, 3], demandOption: true, describe: '暗号化パターン' })
  .option('totalRows', { type: 'number', default: CONFIG.totalRows })
  .option('userCount', { type: 'number', default: CONFIG.userCount })
  .option('batchSize', { type: 'number', default: CONFIG.batchSize })
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
const buildMessage = () => {
  let text = '';
  while (text.length < 30) {
    text = text ? `${text} ${faker.lorem.sentence()}` : faker.lorem.sentence();
    if (text.length >= 500) return text.slice(0, 500);
  }
  return text.length > 500 ? text.slice(0, 500) : text;
};
const generateDataset = (totalRows, userCount) => {
  const users = Array.from({ length: userCount }, () => ulid());
  return Array.from({ length: totalRows }, (_, i) => ({ id: ulid(), createdBy: users[i % userCount], message: buildMessage() }));
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
const truncatePosts = (pool) => pool.query('TRUNCATE TABLE posts');
const bulkInsert = async (pool, rows, pattern, batchSize) => {
  const commonKey = pattern === 2 ? CONFIG.commonKey : null;
  for (let offset = 0; offset < rows.length; offset += batchSize) {
    const batch = rows.slice(offset, offset + batchSize);
    const placeholders = [];
    const values = [];
    let param = 1;
    for (const row of batch) {
      if (pattern === 1) {
        placeholders.push(`($${param}, $${param + 1}, $${param + 2}, NOW())`);
        values.push(row.id, row.createdBy, Buffer.from(row.message, 'utf8'));
        param += 3;
      } else {
        const key = pattern === 2 ? commonKey : deriveUserKey(row.createdBy);
        placeholders.push(`($${param}, $${param + 1}, pgp_sym_encrypt($${param + 2}, $${param + 3}), NOW())`);
        values.push(row.id, row.createdBy, row.message, key);
        param += 4;
      }
    }
    await pool.query(`INSERT INTO posts (id, created_by, message, created_at) VALUES ${placeholders.join(',')}`, values);
  }
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
const writeJsonResult = async (pattern, durations, throughput, params) => {
  const dir = ensureResultsDir();
  const timestamp = new Date().toISOString().replace(/[:.]/g, '-');
  const payload = {
    scenario: 'insert',
    pattern,
    recordedAt: new Date().toISOString(),
    parameters: params,
    durationsMs: durations,
    throughputRowsPerSec: throughput,
    stats: { meanMs: mean(durations), medianMs: median(durations), stddevMs: stddev(durations) }
  };
  const file = path.join(dir, `${timestamp}-insert-p${pattern}.json`);
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
const runBenchmark = async (task, warmup, runs) => {
  for (let i = 0; i < warmup; i += 1) await task();
  const durations = [];
  for (let i = 0; i < runs; i += 1) {
    const start = performance.now();
    await task();
    durations.push(performance.now() - start);
  }
  return durations;
};
async function main() {
  ensureKeys(argv.pattern);
  const pool = new Pool({ connectionString: CONFIG.url });
  const dataset = generateDataset(argv.totalRows, argv.userCount);
  const task = async () => { await truncatePosts(pool); await bulkInsert(pool, dataset, argv.pattern, argv.batchSize); };
  const durations = await runBenchmark(task, argv.warmup, argv.runs);
  const throughput = durations.map((d) => dataset.length / (d / 1000));
  const { payload } = await writeJsonResult(argv.pattern, durations, throughput, {
    totalRows: dataset.length,
    userCount: argv.userCount,
    batchSize: argv.batchSize,
    warmup: argv.warmup,
    runs: argv.runs
  });
  await appendSummaryRows([
    { scenario: 'insert', pattern: argv.pattern, metric: 'median_ms', value: payload.stats.medianMs.toFixed(2) },
    { scenario: 'insert', pattern: argv.pattern, metric: 'mean_ms', value: payload.stats.meanMs.toFixed(2) },
    { scenario: 'insert', pattern: argv.pattern, metric: 'stddev_ms', value: payload.stats.stddevMs.toFixed(2) },
    { scenario: 'insert', pattern: argv.pattern, metric: 'median_rows_per_sec', value: median(throughput).toFixed(2), unit: 'rows/s' }
  ]);
  await pool.end();
  console.log(`INSERTベンチ完了: パターン${argv.pattern} / 実行${argv.runs}回`);
}
main().catch((error) => {
  console.error('INSERTベンチでエラーが発生しました:', error);
  process.exit(1);
});
