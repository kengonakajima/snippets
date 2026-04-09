// agentOS demo: ホストPCのディレクトリをVM内にマウント
import { AgentOs, createHostDirBackend } from "@rivet-dev/agent-os-core";

const vm = await AgentOs.create();

// ホストの ~/agentOS をVM内の /host にマウント（読み取り専用）
const hostFs = createHostDirBackend({ hostPath: "/Users/ringo/agentOS" });
vm.mountFs("/host", hostFs, { readOnly: true });

// VM内からホストのファイルを読む
const entries = await vm.readdir("/host");
console.log("/host/:", entries.filter((e: string) => e !== "." && e !== ".."));

const pkg = await vm.readFile("/host/package.json");
console.log("\npackage.json:", new TextDecoder().decode(pkg));

const demo = await vm.readFile("/host/demo.ts");
console.log("demo.ts (先頭3行):");
console.log(new TextDecoder().decode(demo).split("\n").slice(0, 3).join("\n"));

await vm.dispose();
