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
  if (raw === undefined || raw === null || raw === '') return fallback;
  const value = Number(raw);
  if (!Number.isFinite(value) || value <= 0) {
    throw new Error(`${name} は正の数で指定してください (現在: ${raw})`);
  }
  return value;
};

const CONFIG = {
  url: process.env.DATABASE_URL || process.env.PG_CONNECTION_STRING || 'postgres://localhost:5432/postgres',
  resultsDir: process.env.RESULTS_DIR || 'results',
  vectorDim: numEnv('VECTOR_DIM', numEnv('BENCH_VECTOR_DIM', 1536)),
  ivfflatProbes: numEnv('VECTOR_IVFFLAT_PROBES', 10),
  requests: numEnv('VECTOR_SEARCH_REQUESTS', 500),
  queryPool: numEnv('VECTOR_QUERY_POOL', 500),
  commonKey: process.env.COMMON_KEY,
  userBaseKey: process.env.USER_BASE_KEY
};

const argv = yargs(hideBin(process.argv))
  .option('pattern', { alias: 'p', type: 'number', choices: [1, 2, 3], demandOption: true, describe: '暗号化パターン' })
  .option('requests', { type: 'number', default: CONFIG.requests, describe: '計測リクエスト数' })
  .option('warmup', { type: 'number', default: 1 })
  .option('runs', { type: 'number', default: 3 })
  .option('k', { type: 'number', default: 10, describe: '近傍取得件数' })
  .option('queryPool', { type: 'number', default: CONFIG.queryPool, describe: 'クエリベクトルの事前サンプル数' })
  .option('probes', { type: 'number', default: CONFIG.ivfflatProbes, describe: 'ivfflat.probes の設定値' })
  .help()
  .strict()
  .parseSync();

const ensureResultsDir = () => {
  const dir = path.resolve(process.cwd(), CONFIG.resultsDir);
  if (!fs.existsSync(dir)) fs.mkdirSync(dir, { recursive: true });
  return dir;
};

const ensureKeys = (pattern) => {
  if (pattern >= 2 && !CONFIG.commonKey) throw new Error('COMMON_KEY が設定されていません');
  if (pattern === 3 && !CONFIG.userBaseKey) throw new Error('USER_BASE_KEY が設定されていません');
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

const normalizeVectorLiteral = (value) => {
  if (typeof value === 'string') return value;
  if (Array.isArray(value)) return `[${value.join(',')}]`;
  throw new Error('message_embedding を文字列化できませんでした');
};

const fetchQueryVectors = async (pool, limit) => {
  const { rows } = await pool.query('SELECT id, message_embedding FROM posts LIMIT $1', [limit]);
  if (!rows.length) {
    throw new Error('postsテーブルにデータがありません。先に seed を実行してください。');
  }
  return rows.map((row) => ({ id: row.id, vector: normalizeVectorLiteral(row.message_embedding) }));
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

const writeJsonResult = async (pattern, durations, latencyStats, hitRate, throughput, params) => {
  const dir = ensureResultsDir();
  const timestamp = new Date().toISOString().replace(/[:.]/g, '-');
  const payload = {
    scenario: 'vector-search',
    pattern,
    recordedAt: new Date().toISOString(),
    parameters: params,
    durationsMs: durations,
    perRequestLatencyMs: latencyStats,
    hitRate,
    throughputReqPerSec: throughput,
    stats: { meanMs: mean(durations), medianMs: median(durations), stddevMs: stddev(durations) }
  };
  const file = path.join(dir, `${timestamp}-vector-search-p${pattern}.json`);
  await fsp.writeFile(file, JSON.stringify(payload, null, 2));
  return { file, payload };
};

const appendSummaryRows = async (rows) => {
  if (!rows.length) return;
  const dir = ensureResultsDir();
  const csv = path.join(dir, 'summary.csv');
  try {
    await fsp.access(csv);
  } catch (_) {
    await fsp.writeFile(csv, 'timestamp,scenario,pattern,metric,value,unit\n');
  }
  const timestamp = new Date().toISOString();
  const lines = rows.map(({ scenario, pattern, metric, value, unit = '' }) => [timestamp, scenario, `p${pattern}`, metric, value, unit]
    .map((cell) => (typeof cell === 'string' && cell.includes(',') ? `"${cell}"` : cell)).join(','));
  await fsp.appendFile(csv, `${lines.join('\n')}\n`);
};

async function main() {
  ensureKeys(argv.pattern);
  const pool = new Pool({ connectionString: CONFIG.url });
  const queryVectors = await fetchQueryVectors(pool, argv.queryPool);
  const task = async () => {
    const latencies = [];
    let hitCount = 0;
    const client = await pool.connect();
    try {
      await client.query(`SET ivfflat.probes = ${argv.probes}`);
      for (let i = 0; i < argv.requests; i += 1) {
        const target = queryVectors[Math.floor(Math.random() * queryVectors.length)];
        const start = performance.now();
        const { rows } = await client.query(
          'SELECT id FROM posts ORDER BY message_embedding <-> $1 LIMIT $2',
          [target.vector, argv.k]
        );
        const end = performance.now();
        latencies.push(end - start);
        if (rows.some((row) => row.id === target.id)) hitCount += 1;
      }
    } finally {
      client.release();
    }
    return { latencies, hitCount };
  };

  const { durations, results } = await runBenchmark(task, argv.warmup, argv.runs);
  const latencies = results.flatMap((r) => r.latencies);
  const totalHits = results.reduce((sum, r) => sum + r.hitCount, 0);
  const requestsTotal = argv.requests * argv.runs;
  const latencyStats = { meanMs: mean(latencies), medianMs: median(latencies), stddevMs: stddev(latencies) };
  const throughput = durations.map((d) => argv.requests / (d / 1000));
  const hitRate = {
    total: requestsTotal,
    hits: totalHits,
    ratio: requestsTotal ? totalHits / requestsTotal : 0
  };

  const { payload } = await writeJsonResult(argv.pattern, durations, latencyStats, hitRate, throughput, {
    requests: argv.requests,
    warmup: argv.warmup,
    runs: argv.runs,
    k: argv.k,
    probes: argv.probes,
    queryPool: argv.queryPool,
    vectorDim: CONFIG.vectorDim
  });

  await appendSummaryRows([
    { scenario: 'vector-search', pattern: argv.pattern, metric: 'median_ms', value: latencyStats.medianMs.toFixed(4) },
    { scenario: 'vector-search', pattern: argv.pattern, metric: 'mean_ms', value: latencyStats.meanMs.toFixed(4) },
    { scenario: 'vector-search', pattern: argv.pattern, metric: 'stddev_ms', value: latencyStats.stddevMs.toFixed(4) },
    { scenario: 'vector-search', pattern: argv.pattern, metric: 'median_req_per_sec', value: median(throughput).toFixed(2), unit: 'req/s' },
    { scenario: 'vector-search', pattern: argv.pattern, metric: 'hit_ratio', value: hitRate.ratio.toFixed(4) }
  ]);

  await pool.end();
  console.log(`ベクトル検索ベンチ完了: パターン${argv.pattern}`);
}

main().catch((error) => {
  console.error('ベクトル検索ベンチでエラーが発生しました:', error);
  process.exit(1);
});
