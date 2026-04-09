// agentOS minimal demo: VM内でファイルシステム操作
import { AgentOs } from "@rivet-dev/agent-os-core";

// 1. VMを作成
console.log("--- VM作成 ---");
const vm = await AgentOs.create();

// 2. ファイル書き込み・読み取り
console.log("\n--- ファイル操作 ---");
await vm.writeFile("/hello.txt", "Hello from agentOS!");
const content = await vm.readFile("/hello.txt");
console.log("読み取り:", new TextDecoder().decode(content));

// 3. ディレクトリ操作
console.log("\n--- ディレクトリ操作 ---");
await vm.mkdir("/project");
await vm.mkdir("/project/src");
await vm.writeFile("/project/src/main.ts", 'export const greeting = "こんにちは agentOS";');
await vm.writeFile("/project/README.md", "# agentOS Demo Project");

const entries = await vm.readdir("/project");
console.log("project/:", entries.filter((e: string) => e !== "." && e !== ".."));

const stat = await vm.stat("/project/src/main.ts");
console.log("main.ts size:", stat.size, "bytes");

// 4. 再帰的ディレクトリ一覧
const tree = await vm.readdirRecursive("/project", { maxDepth: 3 });
console.log("ツリー:", tree);

// 5. ファイル移動・存在確認・削除
console.log("\n--- 移動・削除 ---");
await vm.move("/project/README.md", "/project/docs.md");
console.log("docs.md exists:", await vm.exists("/project/docs.md"));
console.log("README.md exists:", await vm.exists("/project/README.md"));

await vm.delete("/project/docs.md");
console.log("docs.md after delete:", await vm.exists("/project/docs.md"));

// 6. クリーンアップ
await vm.delete("/project", { recursive: true });
console.log("project after delete:", await vm.exists("/project"));

await vm.dispose();
console.log("\n--- 完了 ---");
