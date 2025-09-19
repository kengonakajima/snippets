#!/usr/bin/env node
const crypto = require('crypto');
const { Pool } = require('pg');
const { ulid } = require('ulid');
const { faker } = require('@faker-js/faker');
const dotenv = require('dotenv');
const yargs = require('yargs/yargs');
const { hideBin } = require('yargs/helpers');

dotenv.config();

function numberEnv(name, fallback) {
  const raw = process.env[name];
  if (!raw) return fallback;
  const value = Number(raw);
  if (Number.isNaN(value)) {
    throw new Error(`環境変数${name}は数値である必要があります (現在: ${raw})`);
  }
  return value;
}

const DEFAULTS = {
  databaseUrl: process.env.DATABASE_URL || process.env.PG_CONNECTION_STRING || 'postgres://localhost:5432/postgres',
  totalRows: numberEnv('BENCH_TOTAL_ROWS', 10000),
  userCount: numberEnv('BENCH_USER_COUNT', 1000),
  batchSize: numberEnv('BENCH_BATCH_SIZE', 500),
  commonKey: process.env.COMMON_KEY,
  userBaseKey: process.env.USER_BASE_KEY,
  pbkdf2Iterations: numberEnv('PBKDF2_ITERATIONS', 100000),
  pbkdf2KeyLength: numberEnv('PBKDF2_KEY_LENGTH', 32),
  pbkdf2Digest: process.env.PBKDF2_DIGEST || 'sha256',
  vectorDim: numberEnv('VECTOR_DIM', numberEnv('BENCH_VECTOR_DIM', 1536))
};

const argv = yargs(hideBin(process.argv))
  .option('pattern', {
    alias: 'p',
    describe: '暗号化パターン (1=なし,2=共通鍵,3=ユーザー別鍵)',
    type: 'number',
    demandOption: true,
    choices: [1, 2, 3]
  })
  .option('totalRows', {
    describe: '投入する総行数',
    type: 'number',
    default: DEFAULTS.totalRows
  })
  .option('userCount', {
    describe: 'ユーザー数 (created_byの種類)',
    type: 'number',
    default: DEFAULTS.userCount
  })
  .option('batchSize', {
    describe: 'INSERTバッチサイズ',
    type: 'number',
    default: DEFAULTS.batchSize
  })
  .help()
  .strict()
  .parseSync();

function validateKeys(pattern) {
  if (pattern >= 2 && !DEFAULTS.commonKey) {
    throw new Error('COMMON_KEY が設定されていません');
  }
  if (pattern === 3 && !DEFAULTS.userBaseKey) {
    throw new Error('USER_BASE_KEY が設定されていません');
  }
}

function generateDataset(totalRows, userCount) {
  const users = Array.from({ length: userCount }, () => ulid());
  const rows = [];
  for (let i = 0; i < totalRows; i += 1) {
    const message = buildMessage();
    const embedding = buildEmbedding(message);
    rows.push({
      id: ulid(),
      createdBy: users[i % userCount],
      message,
      embedding
    });
  }
  return rows;
}

function buildMessage() {
  let text = '';
  while (text.length < 30) {
    const sentence = faker.lorem.sentence();
    text = text ? `${text} ${sentence}` : sentence;
    if (text.length >= 500) {
      text = text.slice(0, 500);
      break;
    }
  }
  if (text.length > 500) {
    return text.slice(0, 500);
  }
  return text;
}

const userKeyCache = new Map();

function deriveUserKey(userId) {
  if (userKeyCache.has(userId)) {
    return userKeyCache.get(userId);
  }
  const key = crypto
    .pbkdf2Sync(
      DEFAULTS.userBaseKey,
      userId,
      DEFAULTS.pbkdf2Iterations,
      DEFAULTS.pbkdf2KeyLength,
      DEFAULTS.pbkdf2Digest
    )
    .toString('hex');
  userKeyCache.set(userId, key);
  return key;
}

function buildEmbedding(message) {
  const values = [];
  let counter = 0;
  let buffer = Buffer.alloc(0);
  while (values.length < DEFAULTS.vectorDim) {
    const hash = crypto.createHash('sha256').update(message).update(String(counter)).digest();
    buffer = Buffer.concat([buffer, hash]);
    counter += 1;
    while (buffer.length >= 4 && values.length < DEFAULTS.vectorDim) {
      const chunk = buffer.subarray(0, 4);
      buffer = buffer.subarray(4);
      const int = chunk.readUInt32BE(0);
      const value = (int / 0xffffffff) * 2 - 1; // [-1, 1] の範囲に正規化
      values.push(Number(value.toFixed(6)));
    }
  }
  return `[${values.join(',')}]`;
}

async function bulkInsert(pool, rows, pattern, batchSize) {
  const commonKey = pattern === 2 ? DEFAULTS.commonKey : null;
  for (let offset = 0; offset < rows.length; offset += batchSize) {
    const batch = rows.slice(offset, offset + batchSize);
    const values = [];
    const placeholders = [];
    let param = 1;
    for (const row of batch) {
      if (pattern === 1) {
        placeholders.push(`($${param}, $${param + 1}, $${param + 2}, $${param + 3}, NOW())`);
        values.push(row.id, row.createdBy, Buffer.from(row.message, 'utf8'), row.embedding);
        param += 4;
      } else if (pattern === 2) {
        placeholders.push(`($${param}, $${param + 1}, pgp_sym_encrypt($${param + 2}, $${param + 3}), $${param + 4}, NOW())`);
        values.push(row.id, row.createdBy, row.message, commonKey, row.embedding);
        param += 5;
      } else {
        const key = deriveUserKey(row.createdBy);
        placeholders.push(`($${param}, $${param + 1}, pgp_sym_encrypt($${param + 2}, $${param + 3}), $${param + 4}, NOW())`);
        values.push(row.id, row.createdBy, row.message, key, row.embedding);
        param += 5;
      }
    }
    const sql = `INSERT INTO posts (id, created_by, message, message_embedding, created_at) VALUES ${placeholders.join(',')}`;
    await pool.query(sql, values);
  }
}

async function main() {
  validateKeys(argv.pattern);
  const pool = new Pool({ connectionString: DEFAULTS.databaseUrl });
  const client = await pool.connect();
  try {
    await client.query('BEGIN');
    await client.query('TRUNCATE TABLE posts');
    await client.query('COMMIT');
  } catch (error) {
    await client.query('ROLLBACK');
    client.release();
    await pool.end();
    throw error;
  }
  client.release();

  const dataset = generateDataset(argv.totalRows, argv.userCount);
  console.log(`データ生成完了: ${dataset.length}行 (ユーザー数 ${argv.userCount})`);

  try {
    await bulkInsert(pool, dataset, argv.pattern, argv.batchSize);
    console.log(`シード完了: パターン${argv.pattern} / ${dataset.length}行`);
  } finally {
    await pool.end();
  }
}

main().catch((error) => {
  console.error('シード処理に失敗しました:', error);
  process.exit(1);
});
