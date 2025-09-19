#!/usr/bin/env node
const fs = require('fs');
const fsp = require('fs/promises');
const path = require('path');
const { performance } = require('perf_hooks');
const { Pool } = require('pg');
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
  resultsDir: process.env.RESULTS_DIR || 'results',
  requests: numEnv('TRGM_SEARCH_REQUESTS', 500),
  similarityThreshold: Number(process.env.TRGM_SIMILARITY_THRESHOLD ?? 0.3)
};

const argv = yargs(hideBin(process.argv))
  .option('pattern', { alias: 'p', type: 'number', default: 1, describe: '暗号化パターン (pg_trgmベンチは1のみサポート)' })
  .option('requests', { type: 'number', default: CONFIG.requests, describe: '計測リクエスト数' })
  .option('warmup', { type: 'number', default: 1 })
  .option('runs', { type: 'number', default: 3 })
  .option('threshold', { type: 'number', default: CONFIG.similarityThreshold, describe: 'pg_trgm.similarity_threshold の設定値' })
  .option('limit', { type: 'number', default: 20, describe: 'クエリ結果のLIMIT件数' })
  .help()
  .strict()
  .parseSync();

if (argv.pattern !== 1) {
  throw new Error('pg_trgmベンチはパターン1 (平文) のみサポートしています');
}

const ensureResultsDir = () => {
  const dir = path.resolve(process.cwd(), CONFIG.resultsDir);
  if (!fs.existsSync(dir)) fs.mkdirSync(dir, { recursive: true });
  return dir;
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

const tokenize = (message) => {
  const words = message.split(/\s+/).filter((w) => w.length >= 4);
  const base = words.length ? words[Math.floor(Math.random() * words.length)] : message;
  if (!base) return null;
  const start = Math.floor(Math.random() * Math.max(1, base.length - 4));
  const len = Math.min(5, base.length - start);
  return base.slice(start, start + len) || base;
};

const collectTokens = async (pool, limit = 500) => {
  const { rows } = await pool.query('SELECT message_plain FROM posts WHERE message_plain IS NOT NULL LIMIT $1', [limit]);
  return rows.map((row) => tokenize(row.message_plain)).filter(Boolean);
};

const writeJsonResult = async (durations, latencyStats, throughput, params) => {
  const dir = ensureResultsDir();
  const timestamp = new Date().toISOString().replace(/[:.]/g, '-');
  const payload = {
    scenario: 'trgm-search',
    pattern: 1,
    recordedAt: new Date().toISOString(),
    parameters: params,
    durationsMs: durations,
    perRequestLatencyMs: latencyStats,
    throughputReqPerSec: throughput,
    stats: { meanMs: mean(durations), medianMs: median(durations), stddevMs: stddev(durations) }
  };
  const file = path.join(dir, `${timestamp}-trgm-search-p1.json`);
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
  const pool = new Pool({ connectionString: CONFIG.url });
  const tokens = await collectTokens(pool);
  if (!tokens.length) {
    throw new Error('pg_trgm検索用トークンが抽出できませんでした。パターン1で seed を実行してください。');
  }

  const task = async () => {
    const latencies = [];
    const matches = [];
    const client = await pool.connect();
    try {
      await client.query(`SET pg_trgm.similarity_threshold = ${argv.threshold}`);
      for (let i = 0; i < argv.requests; i += 1) {
        const sample = tokens[Math.floor(Math.random() * tokens.length)];
        const likePattern = `%${sample}%`;
        const start = performance.now();
        const { rows } = await client.query(
          `SELECT id
             FROM posts
            WHERE message_plain ILIKE $1
            ORDER BY similarity(message_plain, $2) DESC
            LIMIT $3`,
          [likePattern, sample, argv.limit]
        );
        const end = performance.now();
        latencies.push(end - start);
        matches.push(rows.length);
      }
    } finally {
      client.release();
    }
    return { latencies, matches };
  };

  const { durations, results } = await runBenchmark(task, argv.warmup, argv.runs);
  const latencies = results.flatMap((r) => r.latencies);
  const matchStats = {
    mean: mean(results.flatMap((r) => r.matches)),
    median: median(results.flatMap((r) => r.matches)),
    stddev: stddev(results.flatMap((r) => r.matches))
  };
  const latencyStats = { meanMs: mean(latencies), medianMs: median(latencies), stddevMs: stddev(latencies) };
  const throughput = durations.map((d) => argv.requests / (d / 1000));

  const { payload } = await writeJsonResult(durations, latencyStats, throughput, {
    requests: argv.requests,
    warmup: argv.warmup,
    runs: argv.runs,
    threshold: argv.threshold,
    limit: argv.limit
  });

  await appendSummaryRows([
    { scenario: 'trgm-search', pattern: payload.pattern, metric: 'median_ms', value: latencyStats.medianMs.toFixed(4) },
    { scenario: 'trgm-search', pattern: payload.pattern, metric: 'mean_ms', value: latencyStats.meanMs.toFixed(4) },
    { scenario: 'trgm-search', pattern: payload.pattern, metric: 'stddev_ms', value: latencyStats.stddevMs.toFixed(4) },
    { scenario: 'trgm-search', pattern: payload.pattern, metric: 'median_req_per_sec', value: median(throughput).toFixed(2), unit: 'req/s' },
    { scenario: 'trgm-search', pattern: payload.pattern, metric: 'median_match_count', value: matchStats.median.toFixed(2) }
  ]);

  await pool.end();
  console.log('pg_trgm検索ベンチ完了: パターン1');
}

main().catch((error) => {
  console.error('pg_trgm検索ベンチでエラーが発生しました:', error);
  process.exit(1);
});
