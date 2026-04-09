// agentOS demo: VM内でClaude AIエージェントにプロンプトを送る（計測付き）
import { AgentOs } from "@rivet-dev/agent-os-core";
import { allowAll } from "@secure-exec/core";
import common from "@rivet-dev/agent-os-common";
import pi from "@rivet-dev/agent-os-pi";
import { readFileSync } from "fs";

// .envからAPIキーを読む
const envContent = readFileSync(".env", "utf-8");
const apiKey = envContent.match(/ANTHROPIC_API_KEY=(.+)/)?.[1]?.trim();

const t0 = performance.now();

const vm = await AgentOs.create({
  software: [common, pi],
  permissions: allowAll,
});
const t1 = performance.now();
console.log(`VM作成: ${(t1 - t0).toFixed(0)}ms`);

const { sessionId } = await vm.createSession("pi", {
  env: { ANTHROPIC_API_KEY: apiKey! },
});
const t2 = performance.now();
console.log(`セッション作成: ${(t2 - t1).toFixed(0)}ms`);

// イベント表示
vm.onSessionEvent(sessionId, (event: any) => {
  if (event.type === "message_chunk") {
    process.stdout.write(event.data?.text ?? "");
  }
});

const result = await vm.prompt(sessionId, "Write a haiku about programming to /home/user/haiku.txt");
const t3 = performance.now();
console.log(`\nプロンプト応答: ${(t3 - t2).toFixed(0)}ms`);

// エージェントが作ったファイルを読む
const exists = await vm.exists("/home/user/haiku.txt");
if (exists) {
  const content = await vm.readFile("/home/user/haiku.txt");
  console.log("haiku.txt:", new TextDecoder().decode(content));
} else {
  console.log("ファイルは作成されませんでした");
}

vm.closeSession(sessionId);
await vm.dispose();
const t4 = performance.now();
console.log(`クリーンアップ: ${(t4 - t3).toFixed(0)}ms`);
console.log(`合計: ${(t4 - t0).toFixed(0)}ms`);
