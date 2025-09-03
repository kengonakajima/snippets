import { webcrypto as nodeCrypto } from "crypto";
import { performance } from "perf_hooks";

async function main() {
  // Node v16: WebCrypto を補完
  if (!globalThis.crypto) {
    globalThis.crypto = nodeCrypto;
  }

  // ts-mls を動的 import（WebCrypto 前提のため）
  const m = await import("ts-mls");
  const {
    defaultCapabilities,
    defaultLifetime,
    getCiphersuiteFromName,
    getCiphersuiteImpl,
    generateKeyPackage,
    createGroup,
    createApplicationMessage,
    createCommit,
    encodeMlsMessage,
    decodeMlsMessage,
    emptyPskIndex,
  } = m;

  // WebCryptoで動作するスイート（P-256）
  const csName = "MLS_128_DHKEMP256_AES128GCM_SHA256_P256";
  const cs = await getCiphersuiteImpl(getCiphersuiteFromName(csName));

  // Alice/Bob の KeyPackage 生成
  const aliceCred = { credentialType: "basic", identity: utf8("alice") };
  const bobCred = { credentialType: "basic", identity: utf8("bob") };

  const { publicPackage: aliceKP, privatePackage: alicePriv } = await generateKeyPackage(
    aliceCred,
    defaultCapabilities(),
    defaultLifetime,
    [],
    cs
  );
  const { publicPackage: bobKP, privatePackage: bobPriv } = await generateKeyPackage(
    bobCred,
    defaultCapabilities(),
    defaultLifetime,
    [],
    cs
  );

  // グループ作成とBob追加
  const groupId = utf8("bench-group");
  let aliceState = await createGroup(groupId, aliceKP, alicePriv, [], cs);
  const addBobProposal = { proposalType: "add", add: { keyPackage: bobKP } };
  const { newState: aliceAfterCommit, welcome } = await createCommit(
    aliceState,
    emptyPskIndex,
    true,
    [addBobProposal],
    cs,
    true
  );
  aliceState = aliceAfterCommit;
  if (!welcome) throw new Error("Welcome not generated");

  // Bob の join（復号は今回の計測対象外）
  await (await import("ts-mls")).joinGroup(welcome, bobKP, bobPriv, emptyPskIndex, cs);

  // テストデータ生成（1000件）: PrivateMessage を作っておく（生成処理は計測外）
  const N = 1000;
  const privateMessages = new Array(N);
  for (let i = 0; i < N; i++) {
    const plaintext = randBytes(64); // 64Bの任意データ
    const res = await createApplicationMessage(aliceState, plaintext, cs);
    aliceState = res.newState; // 送信で状態が進む
    privateMessages[i] = res.privateMessage;
  }

  // エンコード計測（PrivateMessage -> wire bytes）
  const wires = new Array(N);
  let t0 = performance.now();
  for (let i = 0; i < N; i++) {
    wires[i] = encodeMlsMessage({
      version: "mls10",
      wireformat: "mls_private_message",
      privateMessage: privateMessages[i],
    });
  }
  let t1 = performance.now();
  const encodeMs = t1 - t0;

  // デコード計測（wire bytes -> MLSMessage）
  const decoded = new Array(N);
  t0 = performance.now();
  for (let i = 0; i < N; i++) {
    decoded[i] = decodeMlsMessage(wires[i]);
  }
  t1 = performance.now();
  const decodeMs = t1 - t0;

  // 結果出力
  console.log("Items:", N);
  console.log("Encode total(ms):", encodeMs.toFixed(2));
  console.log("Encode throughput(ops/s):", (N / (encodeMs / 1000)).toFixed(2));
  console.log("Decode total(ms):", decodeMs.toFixed(2));
  console.log("Decode throughput(ops/s):", (N / (decodeMs / 1000)).toFixed(2));
}

function utf8(s) {
  return new TextEncoder().encode(s);
}

function randBytes(n) {
  const a = new Uint8Array(n);
  globalThis.crypto.getRandomValues(a);
  return a;
}

main().catch((e) => {
  console.error(e);
  process.exit(1);
});

