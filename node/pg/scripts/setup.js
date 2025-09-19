#!/usr/bin/env node
const { Pool } = require('pg');
const dotenv = require('dotenv');

dotenv.config();

const numEnv = (name, fallback) => {
  const raw = process.env[name];
  if (!raw) return fallback;
  const value = Number(raw);
  if (!Number.isFinite(value) || value <= 0) {
    throw new Error(`${name} は正の数で指定してください (現在: ${raw})`);
  }
  return value;
};

const CONFIG = {
  url: process.env.DATABASE_URL || process.env.PG_CONNECTION_STRING || 'postgres://localhost:5432/postgres',
  vectorDim: numEnv('VECTOR_DIM', numEnv('BENCH_VECTOR_DIM', 1536)),
  ivfflatLists: numEnv('VECTOR_IVFFLAT_LISTS', 100)
};

async function main() {
  const pool = new Pool({ connectionString: CONFIG.url });
  const client = await pool.connect();
  try {
    await client.query('BEGIN');
    await client.query('CREATE EXTENSION IF NOT EXISTS pgcrypto');
    await client.query('CREATE EXTENSION IF NOT EXISTS vector');
    await client.query('CREATE EXTENSION IF NOT EXISTS pg_trgm');
    await client.query(`
      CREATE TABLE IF NOT EXISTS posts (
        id char(26) PRIMARY KEY,
        created_by char(26) NOT NULL,
        message bytea NOT NULL,
        message_plain text,
        message_embedding vector(${CONFIG.vectorDim}) NOT NULL,
        created_at timestamptz NOT NULL DEFAULT NOW()
      )
    `);
    await client.query(`ALTER TABLE posts ADD COLUMN IF NOT EXISTS message_embedding vector(${CONFIG.vectorDim})`);
    await client.query(`ALTER TABLE posts ALTER COLUMN message_embedding TYPE vector(${CONFIG.vectorDim})`);
    await client.query('ALTER TABLE posts ADD COLUMN IF NOT EXISTS message_plain text');
    await client.query('CREATE INDEX IF NOT EXISTS idx_posts_created_by ON posts (created_by)');
    await client.query(
      `CREATE INDEX IF NOT EXISTS idx_posts_message_embedding
         ON posts USING ivfflat (message_embedding vector_l2_ops)
         WITH (lists = ${CONFIG.ivfflatLists})`
    );
    await client.query(
      `CREATE INDEX IF NOT EXISTS idx_posts_message_trgm
         ON posts USING gin (message_plain gin_trgm_ops)`
    );
    await client.query('COMMIT');
  } catch (error) {
    await client.query('ROLLBACK');
    throw error;
  } finally {
    client.release();
    await pool.end();
  }
  console.log('PostgreSQLセットアップが完了しました。pgvector拡張と埋め込み列を確認しました。');
}

main().catch((error) => {
  console.error('セットアップに失敗しました:', error);
  process.exit(1);
});
