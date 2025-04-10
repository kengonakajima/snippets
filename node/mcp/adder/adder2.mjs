import { McpServer, ResourceTemplate } from "@modelcontextprotocol/sdk/server/mcp.js";
import { StdioServerTransport } from "@modelcontextprotocol/sdk/server/stdio.js";
import { z } from "zod";

// 予期しない例外や未処理のPromiseエラーをキャッチし、stderrへ出力する（プロセス終了の防止のため）
process.on('uncaughtException', (err) => {
  console.error("Uncaught exception:", err);
});
process.on('unhandledRejection', (err) => {
  console.error("Unhandled rejection:", err);
});

// サーバー起動時のログをstderrに出力（ハンドシェイク状態の把握のため）
console.error("Starting MCP server (Demo v1.0.0)...");

// MCPサーバーを生成。SDKがinitializeリクエストへの応答など必要なハンドシェイク処理を内部で処理する
const server = new McpServer({
  name: "Demo",
  version: "1.0.0"
  // ※必要に応じてcapabilitiesなどを追加設定できます
});

// 加算ツールを追加
server.tool("add",
  { a: z.number(), b: z.number() },
  async ({ a, b }) => ({
    content: [{ type: "text", text: String(a + b) }]
  })
);

// 動的なあいさつリソースを追加
server.resource(
  "greeting",
  new ResourceTemplate("greeting://{name}", { list: undefined }),
  async (uri, { name }) => ({
    contents: [{
      uri: uri.href,
      text: `Hello, ${name}!`
    }]
  })
);

// STDIN/STDOUTでの通信を行うためのトランスポートを生成
const transport = new StdioServerTransport();

// サーバーとの接続を開始。SDKのconnect()は初期化（initialize）のハンドシェイクを内部で行い、
 // 正常なら永続的な通信状態となるはず。エラー時はcatch節でログ出力する。
try {
  await server.connect(transport);
} catch (err) {
  console.error("Error during server connection:", err);
}

// 接続が完了したら、プロセスが終了しないように無限待機する。
// （SDKの内部処理で終了しないことが前提ですが、念のため永続化）
console.error("MCP server connected. Awaiting JSON-RPC messages...");
await new Promise(() => {}); // 終了しないPromise

