import { webcrypto as nodeCrypto } from "crypto";
import { performance } from "perf_hooks";

function utf8(s) {
  return new TextEncoder().encode(s);
}

function percentile(arr, p) {
  if (arr.length === 0) return 0;
  const idx = Math.min(arr.length - 1, Math.max(0, Math.floor((p / 100) * arr.length)));
  return arr.slice().sort((a, b) => a - b)[idx];
}

async function main() {
  // WebCrypto 補完（Node v23では基本不要だが冗長に対応）
  if (!globalThis.crypto) globalThis.crypto = nodeCrypto;

  const N = Number(process.env.N || 1000); // 総ユーザー数（初期1 + 追加N-1）

  const m = await import("ts-mls");
  const {
    defaultCapabilities,
    defaultLifetime,
    getCiphersuiteFromName,
    getCiphersuiteImpl,
    generateKeyPackage,
    createGroup,
    createCommit,
    emptyPskIndex,
  } = m;

  // P-256 スイート
  const csName = "MLS_128_DHKEMP256_AES128GCM_SHA256_P256";
  const cs = await getCiphersuiteImpl(getCiphersuiteFromName(csName));

  // Alice
  const aliceCred = { credentialType: "basic", identity: utf8("alice") };
  const { publicPackage: aliceKP, privatePackage: alicePriv } = await generateKeyPackage(
    aliceCred,
    defaultCapabilities(),
    defaultLifetime,
    [],
    cs
  );

  // グループ生成
  const groupId = utf8("multiuser-group");
  let state = await createGroup(groupId, aliceKP, alicePriv, [], cs);

  // 追加ユーザーの KeyPackage を事前生成（計測対象外）
  const keyPackages = [];
  for (let i = 1; i < N; i++) {
    const cred = { credentialType: "basic", identity: utf8(`user-${i}`) };
    const { publicPackage } = await generateKeyPackage(
      cred,
      defaultCapabilities(),
      defaultLifetime,
      [],
      cs
    );
    keyPackages.push(publicPackage);
  }

  // 追加とコミット（1ユーザーずつ）を計測
  const times = [];
  let lastLog = 0;
  for (let i = 0; i < keyPackages.length; i++) {
    const addProposal = { proposalType: "add", add: { keyPackage: keyPackages[i] } };
    const t0 = performance.now();
    const { newState } = await createCommit(
      state,
      emptyPskIndex,
      true,
      [addProposal],
      cs
    );
    const t1 = performance.now();
    times.push(t1 - t0);
    state = newState;

    // 100件ごとに進捗出力（軽量）
    const size = 1 + (i + 1);
    if (size - lastLog >= 100) {
      console.log(`size=${size} commit_ms=${(t1 - t0).toFixed(3)}`);
      lastLog = size;
    }
  }

  // 統計
  const total = times.reduce((a, b) => a + b, 0);
  const avg = total / times.length;
  const p50 = percentile(times, 50);
  const p90 = percentile(times, 90);
  const p95 = percentile(times, 95);
  const p99 = percentile(times, 99);

  console.log("--- summary ---");
  console.log("users:", N);
  console.log("commits:", times.length);
  console.log("total_ms:", total.toFixed(2));
  console.log("avg_ms:", avg.toFixed(3));
  console.log("p50_ms:", p50.toFixed(3));
  console.log("p90_ms:", p90.toFixed(3));
  console.log("p95_ms:", p95.toFixed(3));
  console.log("p99_ms:", p99.toFixed(3));
}

main().catch((e) => {
  console.error(e);
  process.exit(1);
});
