const { MongoClient } = require('mongodb');
const sodium = require('libsodium-wrappers-sumo');
const { ulid } = require('ulid');
const { performance } = require('perf_hooks');

const TOTAL = 100000;
const USERS = 1000;
const ID_READS = 10000;
const OWNER_READS = 100;
const BATCH = 1000;
const WORDS = 'lorem ipsum dolor sit amet consectetur adipiscing elit sed do eiusmod tempor incididunt ut labore et dolore magna aliqua quick brown fox jumps over lazy dog database server latency throughput benchmark encryption security node mongodb replica shard query index performance runtime metrics dataset workload'.split(' ');

const randomMessage = () => {
  const len = 30 + Math.floor(Math.random() * 471);
  let s = '';
  while (s.length < len) s += (s ? ' ' : '') + WORDS[Math.floor(Math.random() * WORDS.length)];
  return s.slice(0, len);
};

(async () => {
  await sodium.ready;
  const key = sodium.randombytes_buf(sodium.crypto_secretbox_KEYBYTES);
  const toCipher = (plain) => {
    const nonce = sodium.randombytes_buf(sodium.crypto_secretbox_NONCEBYTES);
    const cipher = sodium.crypto_secretbox_easy(sodium.from_string(plain), nonce, key);
    return Buffer.concat([Buffer.from(nonce), Buffer.from(cipher)]).toString('base64');
  };

  const uri = process.env.MONGO_URI || 'mongodb://127.0.0.1:27017';
  const client = new MongoClient(uri, { maxPoolSize: 32 });
  const ids = [];
  const userIds = Array.from({ length: USERS }, () => ulid());

  try {
    await client.connect();
    const col = client.db('benchdb').collection('posts');
    await col.drop().catch(() => {});
    await col.createIndex({ created_by: 1 });

    const insertStart = performance.now();
    for (let i = 0; i < TOTAL; i += BATCH) {
      const batch = [];
      for (let j = 0; j < BATCH && i + j < TOTAL; j++) {
        const id = ulid();
        ids.push(id);
        batch.push({
          _id: id,
          id,
          created_by: userIds[Math.floor(Math.random() * USERS)],
          message: toCipher(randomMessage())
        });
      }
      await col.insertMany(batch, { ordered: false });
    }
    console.log(`insert ${TOTAL} docs: ${(performance.now() - insertStart).toFixed(1)} ms`);

    const findIdStart = performance.now();
    for (let i = 0; i < ID_READS; i++) {
      const target = ids[Math.floor(Math.random() * ids.length)];
      await col.findOne({ _id: target }, { projection: { _id: 0 } });
    }
    console.log(`find by id x${ID_READS}: ${(performance.now() - findIdStart).toFixed(1)} ms`);

    const findUserStart = performance.now();
    for (let i = 0; i < OWNER_READS; i++) {
      await col.find({ created_by: userIds[Math.floor(Math.random() * USERS)] }).toArray();
    }
    console.log(`find by created_by x${OWNER_READS}: ${(performance.now() - findUserStart).toFixed(1)} ms`);
  } catch (err) {
    console.error('bench failed:', err.message);
    process.exitCode = 1;
  } finally {
    await client.close().catch(() => {});
  }
})();
