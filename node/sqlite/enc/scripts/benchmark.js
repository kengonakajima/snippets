#!/usr/bin/env node
'use strict';

const fs = require('fs');
const path = require('path');
const { spawnSync } = require('child_process');

const ROOT = path.resolve(__dirname, '..');
const PASSPHRASE = 'correct horse battery staple';
const VEC_EXTENSION = path.join(ROOT, 'sqlite-vec', 'dist', 'vec0.dylib');
const EMBEDDING_DIM = 768;

const SCENARIOS = [
  {
    name: '10k_top10',
    rows: 10000,
    topK: 10,
    insertSql: path.join(ROOT, 'data', 'inserts.sql'),
    testVector: path.join(ROOT, 'data', 'test_vector.json'),
  },
  {
    name: '100k_top1000',
    rows: 100000,
    topK: 1000,
    insertSql: path.join(ROOT, 'data', 'inserts_100k.sql'),
    testVector: path.join(ROOT, 'data', 'test_vector_100k.json'),
  },
];

function findExecutable(name) {
  const pathEnv = process.env.PATH || '';
  const pathExt = process.platform === 'win32' ? (process.env.PATHEXT || '').split(';') : [''];
  for (const p of pathEnv.split(path.delimiter)) {
    if (!p) continue;
    for (const ext of pathExt) {
      const candidate = path.join(p, name + ext);
      if (fs.existsSync(candidate) && fs.statSync(candidate).isFile()) {
        return candidate;
      }
    }
  }
  return null;
}

const SQLCIPHER_BIN = findExecutable('sqlcipher');

function ensureFilesExist(filePath, description) {
  if (!fs.existsSync(filePath)) {
    throw new Error(`${description} が見つかりません: ${filePath}`);
  }
}

function buildInsertScript(key, scenario) {
  const statements = [`.load ${VEC_EXTENSION}`];
  if (key) {
    statements.push(`PRAGMA key = '${key}';`);
  }
  statements.push('PRAGMA journal_mode = WAL;');
  statements.push('DROP TABLE IF EXISTS vec_data;');
  statements.push(`CREATE VIRTUAL TABLE vec_data USING vec0(embedding float[${EMBEDDING_DIM}], hex_id text);`);
  const relativeInsert = path.relative(ROOT, scenario.insertSql);
  const rawInsertPath = relativeInsert && !relativeInsert.startsWith('..')
    ? relativeInsert
    : scenario.insertSql;
  const normalizedInsertPath = rawInsertPath.replace(/\\/g, '/');
  statements.push(`.read ${JSON.stringify(normalizedInsertPath)}`);
  return `${statements.join('\n')}\n`;
}

function buildSelectScript(key, scenario, testVector) {
  const statements = [`.load ${VEC_EXTENSION}`];
  if (key) {
    statements.push(`PRAGMA key = '${key}';`);
  }
  statements.push(
    'SELECT rowid, hex_id, distance\n' +
      'FROM vec_data\n' +
      `WHERE embedding MATCH '${testVector}'\n` +
      'ORDER BY distance\n' +
      `LIMIT ${scenario.topK};`
  );
  return `${statements.join('\n')}\n`;
}

function runSqlcipher(dbPath, script) {
  if (!SQLCIPHER_BIN) {
    throw new Error('sqlcipher が PATH 上に見つかりません');
  }
  const start = process.hrtime.bigint();
  const result = spawnSync(SQLCIPHER_BIN, [dbPath], {
    input: script,
    encoding: 'utf8',
    stdio: ['pipe', 'pipe', 'pipe'],
    cwd: ROOT,
  });
  const durationSeconds = Number(process.hrtime.bigint() - start) / 1e9;

  if (result.error) {
    throw result.error;
  }
  if (typeof result.status !== 'number' || result.status !== 0) {
    const stderr = (result.stderr || '').toString().trim();
    const prefix = script.length > 200 ? `${script.slice(0, 200)}...` : script;
    throw new Error(`SQLCipher 実行に失敗しました: ${stderr}\nScript:\n${prefix}`);
  }

  return { durationSeconds, stdout: result.stdout || '' };
}

function parseTopRows(output) {
  const rows = [];
  const lines = output.split(/\r?\n/);
  for (const line of lines) {
    const trimmed = line.trim();
    if (!trimmed) {
      continue;
    }
    const parts = trimmed.split('|');
    if (parts.length !== 3) {
      continue;
    }
    const [rowidStr, hexId, distanceStr] = parts;
    const rowid = parseInt(rowidStr, 10);
    const distance = Number(distanceStr);
    if (!Number.isNaN(rowid) && !Number.isNaN(distance)) {
      rows.push({ rowid, hex_id: hexId, distance });
    }
  }
  return rows;
}

function benchmarkCase(dbPath, key, scenario, testVector) {
  if (fs.existsSync(dbPath)) {
    fs.rmSync(dbPath);
  }

  const insertScript = buildInsertScript(key, scenario);
  const insertResult = runSqlcipher(dbPath, insertScript);

  const selectScript = buildSelectScript(key, scenario, testVector);
  runSqlcipher(dbPath, selectScript);
  const selectResult = runSqlcipher(dbPath, selectScript);

  const topRows = parseTopRows(selectResult.stdout);
  const dbSizeBytes = fs.statSync(dbPath).size;

  const insertThroughput = scenario.rows / insertResult.durationSeconds;
  const selectLatencyMs = selectResult.durationSeconds * 1000;

  return {
    insertSeconds: insertResult.durationSeconds,
    insertThroughput,
    selectSeconds: selectResult.durationSeconds,
    selectLatencyMs,
    topRows,
    dbSizeBytes,
  };
}

function main() {
  try {
    ensureFilesExist(VEC_EXTENSION, 'sqlite-vec 拡張モジュール');
    for (const scenario of SCENARIOS) {
      ensureFilesExist(scenario.insertSql, `シナリオ ${scenario.name} のINSERT SQL`);
      ensureFilesExist(scenario.testVector, `シナリオ ${scenario.name} のテストベクター`);
    }

    const results = {};
    for (const scenario of SCENARIOS) {
      console.log(`Running scenario ${scenario.name}...`);
      const testVector = fs.readFileSync(scenario.testVector, { encoding: 'ascii' }).trim();

      const scenarioResults = {
        rows: scenario.rows,
        top_k: scenario.topK,
      };

      const cases = {
        plain: { dbPath: path.join(ROOT, `${scenario.name}_plain.db`), key: null },
        encrypted: { dbPath: path.join(ROOT, `${scenario.name}_encrypted.db`), key: PASSPHRASE },
      };

      for (const [caseName, cfg] of Object.entries(cases)) {
        console.log(`  Measuring ${caseName} database...`);
        const caseResult = benchmarkCase(cfg.dbPath, cfg.key, scenario, testVector);
        scenarioResults[caseName] = {
          insert_seconds: caseResult.insertSeconds,
          insert_throughput: caseResult.insertThroughput,
          select_seconds: caseResult.selectSeconds,
          select_latency_ms: caseResult.selectLatencyMs,
          db_size_bytes: caseResult.dbSizeBytes,
          top_rows: caseResult.topRows,
        };
        console.log(
          `    insert=${caseResult.insertSeconds.toFixed(3)}s ` +
            `(${Math.round(caseResult.insertThroughput)} rows/s), ` +
            `select=${caseResult.selectSeconds.toFixed(3)}s`
        );
      }

      results[scenario.name] = scenarioResults;
    }

    const outputPath = path.join(ROOT, 'data', 'benchmark_results.json');
    fs.writeFileSync(outputPath, `${JSON.stringify(results, null, 2)}\n`, { encoding: 'ascii' });
    console.log(`Wrote results to ${outputPath}`);
  } catch (err) {
    console.error(err instanceof Error ? err.message : String(err));
    process.exit(1);
  }
}

if (require.main === module) {
  main();
}

module.exports = {
  SCENARIOS,
  buildInsertScript,
  buildSelectScript,
  benchmarkCase,
  parseTopRows,
};
