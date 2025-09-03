import { webcrypto as nodeCrypto } from "crypto";

function utf8(s) {
  return new TextEncoder().encode(s);
}

function b64(bytes) {
  // Node 16+: Buffer.from(Uint8Array).toString('base64')
  return Buffer.from(bytes).toString("base64");
}

async function main() {
  // Node v16系では WebCrypto がグローバルに無い場合があるため補完
  if (!globalThis.crypto) {
    globalThis.crypto = nodeCrypto;
  }

  // ts-mls は ESMであり、かつWebCrypto前提のため動的import後に使用
  const m = await import("ts-mls");
  const {
    defaultCapabilities,
    defaultLifetime,
    getCiphersuiteFromName,
    getCiphersuiteImpl,
    generateKeyPackage,
    createGroup,
    createApplicationMessage,
    emptyPskIndex,
    createCommit,
    processPrivateMessage,
    encodeMlsMessage,
  } = m;

  // 1) 暗号スイートを選択（X25519/AES-GCM/SHA-256/Ed25519）
  const csName = "MLS_128_DHKEMP256_AES128GCM_SHA256_P256";
  const cs = await getCiphersuiteImpl(getCiphersuiteFromName(csName));

  // 2) それぞれのクライアント（Alice/Bob）のKeyPackageを作成
  const aliceCred = { credentialType: "basic", identity: utf8("alice") };
  const bobCred = { credentialType: "basic", identity: utf8("bob") };

  const { publicPackage: aliceKP, privatePackage: alicePriv } = await generateKeyPackage(
    aliceCred,
    defaultCapabilities(),
    defaultLifetime,
    [],
    cs,
  );

  const { publicPackage: bobKP, privatePackage: bobPriv } = await generateKeyPackage(
    bobCred,
    defaultCapabilities(),
    defaultLifetime,
    [],
    cs,
  );

  // 3) Aliceがグループを作成
  const groupId = utf8("demo-group");
  let aliceState = await createGroup(groupId, aliceKP, alicePriv, [], cs);

  // 4) Bobを追加するコミットを作成（Add提案をextraProposalsとして渡す）
  const addBobProposal = { proposalType: "add", add: { keyPackage: bobKP } };
  const { newState: aliceAfterCommit, welcome, commit } = await createCommit(
    aliceState,
    emptyPskIndex,
    true, // publicMessageとして送る
    [addBobProposal],
    cs,
    true, // ratchetTreeExtension（新規参加者にツリーを共有）
  );
  aliceState = aliceAfterCommit;

  if (!welcome) {
    throw new Error("Welcomeが生成されていません。メンバー追加が反映されていない可能性があります。");
  }

  // 5) BobがWelcomeでグループに参加
  let bobState = await m.joinGroup(
    welcome,
    bobKP,
    bobPriv,
    emptyPskIndex,
    cs,
  );

  // エポックIDを表示（参加後は1以上になっているはず）
  console.log("Epoch(Alice):", aliceState.groupContext.epoch.toString());
  console.log("Epoch(Bob):  ", bobState.groupContext.epoch.toString());

  // 6) Aliceからアプリケーションメッセージを送信（暗号化）
  const plaintext = utf8("Hello MLS from Alice -> Bob");
  const { newState: aliceAfterSend, privateMessage } = await createApplicationMessage(
    aliceState,
    plaintext,
    cs,
  );
  aliceState = aliceAfterSend;

  // 送信用にワイヤ形式へエンコードして表示（暗号化済み）
  const wire = encodeMlsMessage({
    version: "mls10",
    wireformat: "mls_private_message",
    privateMessage,
  });
  console.log("Encrypted MLS message (base64):", b64(wire));

  // 7) Bob側で復号
  const processed = await processPrivateMessage(bobState, privateMessage, emptyPskIndex, cs);
  if (processed.kind !== "applicationMessage") {
    throw new Error("期待したアプリケーションメッセージではありません");
  }
  bobState = processed.newState;
  const decrypted = new TextDecoder().decode(processed.message);
  console.log("Decrypted message:", decrypted);
}

main().catch((e) => {
  console.error(e);
  process.exit(1);
});
