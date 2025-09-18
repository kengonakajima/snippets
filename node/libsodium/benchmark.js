const { performance } = require('perf_hooks');
const sodiumModule = require('libsodium-wrappers');

const LARGE_TEXT_SIZE = 1_000_000; // 1MB
const SMALL_TEXT_COUNT = 10_000;
const SMALL_TEXT_LENGTH = 100;

function createEnglishBuffer(bytes) {
  const chunk = Buffer.from('The quick brown fox jumps over the lazy dog. ');
  const buffer = Buffer.alloc(bytes);
  for (let offset = 0; offset < bytes; offset += chunk.length) {
    const len = Math.min(chunk.length, bytes - offset);
    chunk.copy(buffer, offset, 0, len);
  }
  return buffer;
}

function createSmallBuffers(count, length) {
  const template = 'Pack my box with five dozen liquor jugs. ';
  const repeated = (template + ' ').repeat(Math.ceil((length + 20) / (template.length + 1)) + 1);
  const results = new Array(count);
  for (let i = 0; i < count; i += 1) {
    const prefix = `Msg ${i.toString().padStart(5, '0')} `;
    const text = (prefix + repeated).slice(0, length);
    results[i] = Buffer.from(text, 'utf8');
  }
  return results;
}

(async () => {
  await sodiumModule.ready;
  const sodium = sodiumModule;

  const keyBytesRequired = sodium.crypto_secretbox_KEYBYTES;
  const nonceBytes = sodium.crypto_secretbox_NONCEBYTES;

  const scenarios = [
    { label: '128-bit key', keyBits: 128 },
    { label: '256-bit key', keyBits: 256 },
  ];

  const largeMessage = createEnglishBuffer(LARGE_TEXT_SIZE);
  const smallMessages = createSmallBuffers(SMALL_TEXT_COUNT, SMALL_TEXT_LENGTH);

  const results = [];

  for (const scenario of scenarios) {
    const inputKeyBytes = scenario.keyBits / 8;
    const rawKey = sodium.randombytes_buf(inputKeyBytes);
    const key = rawKey.length === keyBytesRequired
      ? rawKey
      : sodium.crypto_generichash(keyBytesRequired, rawKey);

    const nonceLarge = sodium.randombytes_buf(nonceBytes);

    const startEncLarge = performance.now();
    const cipherLarge = sodium.crypto_secretbox_easy(largeMessage, nonceLarge, key);
    const encLargeMs = performance.now() - startEncLarge;

    const startDecLarge = performance.now();
    const decryptedLarge = sodium.crypto_secretbox_open_easy(cipherLarge, nonceLarge, key);
    const decLargeMs = performance.now() - startDecLarge;
    if (!Buffer.from(decryptedLarge).equals(largeMessage)) {
      throw new Error(`${scenario.label}: 1MBメッセージの検証に失敗しました`);
    }

    const smallRecords = new Array(smallMessages.length);

    const startEncSmall = performance.now();
    for (let i = 0; i < smallMessages.length; i += 1) {
      const nonce = sodium.randombytes_buf(nonceBytes);
      const cipher = sodium.crypto_secretbox_easy(smallMessages[i], nonce, key);
      smallRecords[i] = { nonce, cipher };
    }
    const encSmallMs = performance.now() - startEncSmall;

    const startDecSmall = performance.now();
    for (let i = 0; i < smallRecords.length; i += 1) {
      const { nonce, cipher } = smallRecords[i];
      const plain = sodium.crypto_secretbox_open_easy(cipher, nonce, key);
      if (!Buffer.from(plain).equals(smallMessages[i])) {
        throw new Error(`${scenario.label}: 100文字メッセージ${i}の検証に失敗しました`);
      }
    }
    const decSmallMs = performance.now() - startDecSmall;

    results.push({
      label: scenario.label,
      keyBits: scenario.keyBits,
      large: { encryptMs: encLargeMs, decryptMs: decLargeMs },
      small: { encryptMs: encSmallMs, decryptMs: decSmallMs },
    });
  }

  console.log('libsodium secretbox ベンチマーク');
  console.log(`1) 1MBテキスト 1回\n2) 100文字テキスト ${SMALL_TEXT_COUNT}回`);
  console.log('\n時間はミリ秒 (ms)\n');
  console.table(results.map((result) => ({
    '鍵ラベル': result.label,
    '鍵ビット長': result.keyBits,
    '1MB暗号化': result.large.encryptMs.toFixed(3),
    '1MB復号': result.large.decryptMs.toFixed(3),
    '100文字×暗号化': result.small.encryptMs.toFixed(3),
    '100文字×復号': result.small.decryptMs.toFixed(3),
  })));
})();
