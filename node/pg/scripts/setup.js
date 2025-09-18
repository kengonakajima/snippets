#!/usr/bin/env node
const { Pool } = require('pg');
const dotenv = require('dotenv');

dotenv.config();
const connectionString = process.env.DATABASE_URL || process.env.PG_CONNECTION_STRING || 'postgres://localhost:5432/postgres';

async function main() {
  const pool = new Pool({ connectionString });
  const client = await pool.connect();
  try {
    await client.query('BEGIN');
    await client.query('CREATE EXTENSION IF NOT EXISTS pgcrypto');
    await client.query(`
      CREATE TABLE IF NOT EXISTS posts (
        id char(26) PRIMARY KEY,
        created_by char(26) NOT NULL,
        message bytea NOT NULL,
        created_at timestamptz NOT NULL DEFAULT NOW()
      )
    `);
    await client.query('CREATE INDEX IF NOT EXISTS idx_posts_created_by ON posts (created_by)');
    await client.query('COMMIT');
  } catch (error) {
    await client.query('ROLLBACK');
    throw error;
  } finally {
    client.release();
    await pool.end();
  }
  console.log('PostgreSQLセットアップが完了しました。');
}

main().catch((error) => {
  console.error('セットアップに失敗しました:', error);
  process.exit(1);
});
