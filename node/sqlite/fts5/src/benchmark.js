const fs = require('fs');
const path = require('path');
const sqlite3 = require('@journeyapps/sqlcipher');
const { performance } = require('perf_hooks');

const DATA_DIR = path.join(__dirname, '..', 'data');
const PLAIN_DB = path.join(DATA_DIR, 'plain.db');
const ENCRYPTED_DB = path.join(DATA_DIR, 'encrypted.db');
const ROW_COUNT = 10_000;
const KEYWORD = 'cipherword';
const ITERATIONS = 10;
const ENCRYPTION_KEY = 'passing-sample-key';

function ensureDir(dir) {
  if (!fs.existsSync(dir)) {
    fs.mkdirSync(dir, { recursive: true });
  }
}

function resetDatabases() {
  ensureDir(DATA_DIR);
  for (const filePath of [PLAIN_DB, ENCRYPTED_DB]) {
    if (fs.existsSync(filePath)) {
      fs.unlinkSync(filePath);
    }
  }
}

function randomWord(length) {
  const alph = 'abcdefghijklmnopqrstuvwxyz';
  let word = '';
  for (let i = 0; i < length; i += 1) {
    word += alph[Math.floor(Math.random() * alph.length)];
  }
  return word;
}

function randomSentence(keywordChance = 0.2) {
  const wordCount = 8 + Math.floor(Math.random() * 12);
  const words = [];
  let injected = false;
  for (let i = 0; i < wordCount; i += 1) {
    if (!injected && Math.random() < keywordChance) {
      words.push(KEYWORD);
      injected = true;
    } else {
      const len = 3 + Math.floor(Math.random() * 6);
      words.push(randomWord(len));
    }
  }
  if (!injected) {
    words[Math.floor(Math.random() * words.length)] = KEYWORD;
  }
  return words.join(' ');
}

function generateDataset() {
  const rows = new Array(ROW_COUNT);
  for (let i = 0; i < ROW_COUNT; i += 1) {
    rows[i] = randomSentence();
  }
  return rows;
}

function buildInsertScript(rows) {
  const escaped = rows.map((text) => text.replace(/'/g, "''"));
  const statements = escaped.map((text) => `INSERT INTO docs(content) VALUES ('${text}');`);
  return `BEGIN TRANSACTION;\n${statements.join('\n')}\nCOMMIT;`;
}

function openDatabase(filePath) {
  return new Promise((resolve, reject) => {
    const db = new sqlite3.Database(filePath, (err) => {
      if (err) {
        reject(err);
      } else {
        resolve(db);
      }
    });
  });
}

function exec(db, sql) {
  return new Promise((resolve, reject) => {
    db.exec(sql, (err) => {
      if (err) {
        reject(err);
      } else {
        resolve();
      }
    });
  });
}

function all(db, sql, params = []) {
  return new Promise((resolve, reject) => {
    db.all(sql, params, (err, rows) => {
      if (err) {
        reject(err);
      } else {
        resolve(rows);
      }
    });
  });
}

async function createDatabase({ filePath, key, rows }) {
  const db = await openDatabase(filePath);
  if (key) {
    await exec(db, `PRAGMA key = '${key}';`);
    await exec(db, 'PRAGMA cipher_compatibility = 4;');
  }
  await exec(db, 'PRAGMA journal_mode = WAL;');
  await exec(db, 'PRAGMA synchronous = NORMAL;');
  await exec(db, 'CREATE VIRTUAL TABLE docs USING fts5(content);');
  await exec(db, buildInsertScript(rows));
  return db;
}

async function measureSearch(db, keyword, iterations) {
  await all(db, 'SELECT rowid FROM docs WHERE docs MATCH ?;', [keyword]);
  const measurements = [];
  for (let i = 0; i < iterations; i += 1) {
    const start = performance.now();
    await all(db, 'SELECT rowid, content FROM docs WHERE docs MATCH ?;', [keyword]);
    const end = performance.now();
    measurements.push(end - start);
  }
  const average = measurements.reduce((a, b) => a + b, 0) / measurements.length;
  const min = Math.min(...measurements);
  const max = Math.max(...measurements);
  return { average, min, max, measurements };
}

function formatStats(label, stats) {
  return {
    label,
    averageMs: stats.average,
    minMs: stats.min,
    maxMs: stats.max,
    measurements: stats.measurements,
  };
}

function closeDatabase(db) {
  return new Promise((resolve, reject) => {
    db.close((err) => {
      if (err) {
        reject(err);
      } else {
        resolve();
      }
    });
  });
}

async function main() {
  console.log('=== SQLCipher + FTS5 ベンチマーク ===');
  console.log(`行数: ${ROW_COUNT}, キーワード: "${KEYWORD}"`);
  resetDatabases();
  const rows = generateDataset();

  console.log('\n[1] 平文DBを作成中...');
  const plainDb = await createDatabase({ filePath: PLAIN_DB, key: null, rows });
  console.log('完了。');

  console.log('\n[2] 暗号化DBを作成中...');
  const encryptedDb = await createDatabase({ filePath: ENCRYPTED_DB, key: ENCRYPTION_KEY, rows });
  console.log('完了。');

  console.log('\n[3] 検索時間を計測します (繰り返し回数:', ITERATIONS, ')');
  const plainStats = await measureSearch(plainDb, KEYWORD, ITERATIONS);
  const encryptedStats = await measureSearch(encryptedDb, KEYWORD, ITERATIONS);

  await closeDatabase(plainDb);
  await closeDatabase(encryptedDb);

  const results = [
    formatStats('暗号化なし', plainStats),
    formatStats('暗号化あり', encryptedStats),
  ];

  for (const result of results) {
    console.log(`\n--- ${result.label} ---`);
    console.log(`平均: ${result.averageMs.toFixed(3)} ms`);
    console.log(`最小: ${result.minMs.toFixed(3)} ms`);
    console.log(`最大: ${result.maxMs.toFixed(3)} ms`);
    console.log('詳細(ms):', result.measurements.map((v) => v.toFixed(3)).join(', '));
  }

  const diff = encryptedStats.average - plainStats.average;
  const diffPct = (diff / plainStats.average) * 100;
  console.log('\n=== サマリ ===');
  console.log(`暗号化なし平均: ${plainStats.average.toFixed(3)} ms`);
  console.log(`暗号化あり平均: ${encryptedStats.average.toFixed(3)} ms`);
  console.log(`差分: ${diff.toFixed(3)} ms (${diffPct.toFixed(2)}%)`);
  console.log('\nデータベースファイル:');
  console.log(`  平文: ${PLAIN_DB}`);
  console.log(`  暗号化: ${ENCRYPTED_DB}`);
}

main().catch((err) => {
  console.error('エラーが発生しました:', err);
  process.exitCode = 1;
});
