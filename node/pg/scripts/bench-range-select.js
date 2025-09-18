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
  requests: numEnv('RANGE_SELECT_REQUESTS', 300),
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
const writeJsonResult = async (pattern, durations, latencyStats, rowCount, throughput, params) => {
  const dir = ensureResultsDir();
  const timestamp = new Date().toISOString().replace(/[:.]/g, '-');
  const payload = {
    scenario: 'range-select',
    pattern,
    recordedAt: new Date().toISOString(),
    parameters: params,
    durationsMs: durations,
    perRequestLatencyMs: latencyStats,
    averageRowCount: rowCount,
    throughputReqPerSec: throughput,
    stats: { meanMs: mean(durations), medianMs: median(durations), stddevMs: stddev(durations) }
  };
  const file = path.join(dir, `${timestamp}-range-select-p${pattern}.json`);
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
const fetchUsers = async (pool) => {
  const { rows } = await pool.query('SELECT DISTINCT created_by FROM posts');
  if (!rows.length) throw new Error('postsテーブルにデータがありません。先に seed を実行してください。');
  return rows.map((row) => row.created_by);
};
const selectByUser = async (client, userId, pattern) => {
  if (pattern === 1) {
    const { rows } = await client.query("SELECT id, created_by, convert_from(message, 'utf8') AS message FROM posts WHERE created_by = $1", [userId]);
    return rows;
  }
  if (pattern === 2) {
    const { rows } = await client.query('SELECT id, created_by, pgp_sym_decrypt(message, $2)::text AS message FROM posts WHERE created_by = $1', [userId, CONFIG.commonKey]);
    return rows;
  }
  const key = deriveUserKey(userId);
  const { rows } = await client.query('SELECT id, created_by, pgp_sym_decrypt(message, $2)::text AS message FROM posts WHERE created_by = $1', [userId, key]);
  return rows;
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
  const users = await fetchUsers(pool);
  const task = async () => {
    const latencies = [];
    const rowCounts = [];
    const client = await pool.connect();
    try {
      for (let i = 0; i < argv.requests; i += 1) {
        const userId = users[Math.floor(Math.random() * users.length)];
        const start = performance.now();
        const rows = await selectByUser(client, userId, argv.pattern);
        const end = performance.now();
        latencies.push(end - start);
        rowCounts.push(rows.length);
      }
    } finally {
      client.release();
    }
    return { latencies, rowCounts };
  };
  const { durations, results } = await runBenchmark(task, argv.warmup, argv.runs);
  const latencies = results.flatMap((r) => r.latencies);
  const rowCounts = results.flatMap((r) => r.rowCounts);
  const latencyStats = { meanMs: mean(latencies), medianMs: median(latencies), stddevMs: stddev(latencies) };
  const averageRowCount = mean(rowCounts);
  const throughput = durations.map((d) => argv.requests / (d / 1000));
  await writeJsonResult(argv.pattern, durations, latencyStats, averageRowCount, throughput, {
    requests: argv.requests,
    warmup: argv.warmup,
    runs: argv.runs
  });
  await appendSummaryRows([
    { scenario: 'range-select', pattern: argv.pattern, metric: 'median_ms', value: latencyStats.medianMs.toFixed(4) },
    { scenario: 'range-select', pattern: argv.pattern, metric: 'mean_ms', value: latencyStats.meanMs.toFixed(4) },
    { scenario: 'range-select', pattern: argv.pattern, metric: 'stddev_ms', value: latencyStats.stddevMs.toFixed(4) },
    { scenario: 'range-select', pattern: argv.pattern, metric: 'median_req_per_sec', value: median(throughput).toFixed(2), unit: 'req/s' }
  ]);
  await pool.end();
  console.log(`範囲SELECTベンチ完了: パターン${argv.pattern}`);
}
main().catch((error) => {
  console.error('範囲SELECTベンチでエラーが発生しました:', error);
  process.exit(1);
});
