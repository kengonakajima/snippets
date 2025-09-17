#!/usr/bin/env node
'use strict';

const fs = require('fs');
const path = require('path');

/**
 * Minimal implementation of Python's random.Random using MT19937.
 */
class PythonRandom {
  constructor(seed) {
    this.N = 624;
    this.M = 397;
    this.MATRIX_A = 0x9908b0df;
    this.UPPER_MASK = 0x80000000;
    this.LOWER_MASK = 0x7fffffff;
    this.mt = new Array(this.N).fill(0);
    this.mti = this.N + 1;
    this.seed(seed);
  }

  seed(seed) {
    if (!Number.isInteger(seed)) {
      throw new Error('seed must be an integer');
    }
    let x = seed >>> 0;
    this.mt[0] = x;
    for (let i = 1; i < this.N; i += 1) {
      x = Math.imul(1812433253, this.mt[i - 1] ^ (this.mt[i - 1] >>> 30)) + i;
      this.mt[i] = x >>> 0;
    }
    this.mti = this.N;
  }

  extractNumber() {
    if (this.mti >= this.N) {
      if (this.mti === this.N + 1) {
        this.seed(5489);
      }
      this.twist();
    }
    let y = this.mt[this.mti];
    this.mti += 1;

    y ^= y >>> 11;
    y ^= (y << 7) & 0x9d2c5680;
    y ^= (y << 15) & 0xefc60000;
    y ^= y >>> 18;

    return y >>> 0;
  }

  twist() {
    let y;
    const mag01 = [0, this.MATRIX_A];
    let kk = 0;
    for (; kk < this.N - this.M; kk += 1) {
      y = (this.mt[kk] & this.UPPER_MASK) | (this.mt[kk + 1] & this.LOWER_MASK);
      this.mt[kk] = this.mt[kk + this.M] ^ (y >>> 1) ^ mag01[y & 1];
    }
    for (; kk < this.N - 1; kk += 1) {
      y = (this.mt[kk] & this.UPPER_MASK) | (this.mt[kk + 1] & this.LOWER_MASK);
      this.mt[kk] = this.mt[kk + (this.M - this.N)] ^ (y >>> 1) ^ mag01[y & 1];
    }
    y = (this.mt[this.N - 1] & this.UPPER_MASK) | (this.mt[0] & this.LOWER_MASK);
    this.mt[this.N - 1] = this.mt[this.M - 1] ^ (y >>> 1) ^ mag01[y & 1];
    this.mti = 0;
  }

  random() {
    const a = this.extractNumber() >>> 5;
    const b = this.extractNumber() >>> 6;
    return (a * 67108864 + b) / 9007199254740992;
  }

  uniform(a, b) {
    return a + (b - a) * this.random();
  }

  choice(str) {
    if (str.length === 0) {
      throw new Error('cannot choose from an empty sequence');
    }
    const idx = Math.floor(this.random() * str.length);
    return str.charAt(idx);
  }
}

function parseArgs(argv) {
  const args = {
    rows: 10000,
    dim: 768,
    seed: 42,
  };

  for (let i = 0; i < argv.length; i += 1) {
    const arg = argv[i];
    if (!arg.startsWith('--')) {
      continue;
    }
    const key = arg.slice(2);
    const next = argv[i + 1];
    if (next === undefined || next.startsWith('--')) {
      throw new Error(`オプション --${key} に値が必要です`);
    }
    switch (key) {
      case 'rows':
        args.rows = parseInt(next, 10);
        break;
      case 'dim':
        args.dim = parseInt(next, 10);
        break;
      case 'seed':
        args.seed = parseInt(next, 10);
        break;
      case 'output-sql':
        args.outputSql = next;
        break;
      case 'output-vector':
        args.outputVector = next;
        break;
      default:
        throw new Error(`未知のオプション: --${key}`);
    }
    i += 1;
  }

  if (!Number.isInteger(args.rows) || args.rows <= 0) {
    throw new Error('--rows は正の整数で指定してください');
  }
  if (!Number.isInteger(args.dim) || args.dim <= 0) {
    throw new Error('--dim は正の整数で指定してください');
  }
  if (!Number.isInteger(args.seed)) {
    throw new Error('--seed は整数で指定してください');
  }
  if (!args.outputSql) {
    throw new Error('--output-sql を指定してください');
  }
  if (!args.outputVector) {
    throw new Error('--output-vector を指定してください');
  }

  return {
    rows: args.rows,
    dim: args.dim,
    seed: args.seed,
    outputSql: path.resolve(args.outputSql),
    outputVector: path.resolve(args.outputVector),
  };
}

function generateVector(rng, dim) {
  const values = [];
  for (let i = 0; i < dim; i += 1) {
    values.push(rng.uniform(-1.0, 1.0).toFixed(6));
  }
  return `[${values.join(',')}]`;
}

function generateHex(rng, length = 32) {
  const alphabet = '0123456789abcdef';
  let result = '';
  for (let i = 0; i < length; i += 1) {
    result += rng.choice(alphabet);
  }
  return result;
}

function main() {
  try {
    const options = parseArgs(process.argv.slice(2));
    const rng = new PythonRandom(options.seed);

    fs.mkdirSync(path.dirname(options.outputSql), { recursive: true });
    fs.mkdirSync(path.dirname(options.outputVector), { recursive: true });

    const fd = fs.openSync(options.outputSql, 'w');
    try {
      fs.writeSync(fd, 'BEGIN;\n', null, 'ascii');
      for (let rowId = 1; rowId <= options.rows; rowId += 1) {
        const embedding = generateVector(rng, options.dim);
        const hexValue = generateHex(rng);
        const line = `INSERT INTO vec_data(rowid, embedding, hex_id) VALUES (${rowId}, '${embedding}', '${hexValue}');\n`;
        fs.writeSync(fd, line, null, 'ascii');
      }
      fs.writeSync(fd, 'COMMIT;\n', null, 'ascii');
    } finally {
      fs.closeSync(fd);
    }

    const testVector = generateVector(rng, options.dim);
    fs.writeFileSync(options.outputVector, `${testVector}\n`, { encoding: 'ascii' });
  } catch (err) {
    console.error(err instanceof Error ? err.message : String(err));
    process.exit(1);
  }
}

if (require.main === module) {
  main();
}

module.exports = {
  PythonRandom,
  generateVector,
  generateHex,
};
