import { McpServer, ResourceTemplate } from "@modelcontextprotocol/sdk/server/mcp.js";
import { StdioServerTransport } from "@modelcontextprotocol/sdk/server/stdio.js";
import { z } from "zod";


function countPrimesBelow(x) {
  // 2未満の場合は素数が存在しない
  if (x < 2) {
    return 0;
  }
  
  // 初期化：すべての数値を素数候補とする
  let isPrime = new Array(x).fill(true);
  
  // 0と1は素数ではない
  isPrime[0] = isPrime[1] = false;
  
  // エラトステネスのふるい
  for (let i = 2; i * i < x; i++) {
    if (isPrime[i]) {
      // iが素数なら、iの倍数をすべて除外
      for (let j = i * i; j < x; j += i) {
        isPrime[j] = false;
      }
    }
  }
  
  // 素数の数をカウント
  let count = 0;
  for (let i = 0; i < x; i++) {
    if (isPrime[i]) {
      count++;
    }
  }
  
  return count;
}

// 使用例
//onsole.log(countPrimesBelow(10)); // 4 (2, 3, 5, 7)
//console.log(countPrimesBelow(100)); // 25


// Create an MCP server
const server = new McpServer({
  name: "Demo2",
  version: "1.0.0"
});

// Add an addition tool
server.tool("count-prime",
            { a: z.number() },
            async ({ a }) => ({
              content: [{ type: "text", text: String( countPrimesBelow(a)) }]
            })
);

// Add a dynamic greeting resource
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

// Start receiving messages on stdin and sending messages on stdout
const transport = new StdioServerTransport();
await server.connect(transport);
