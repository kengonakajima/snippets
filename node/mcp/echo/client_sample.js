import { McpClient } from "@modelcontextprotocol/sdk";
import { SSEClientTransport } from "@modelcontextprotocol/sdk";

async function main() {
  // サーバへの接続を設定
  const transport = new SSEClientTransport("http://localhost:3001/sse", "http://localhost:3001/messages");
  const client = new McpClient(transport);
  
  try {
    // サーバに接続
    await client.connect();
    console.log("Connected to server");
    
    // ツールの呼び出し
    const toolResult = await client.useTool("echo", { message: "Hello from client tool!" });
    console.log("Tool result:", toolResult);
    
    // リソースの取得
    const resource = await client.getResource("echo://Hello from client resource!");
    console.log("Resource:", resource);
    
    // プロンプトの送信
    const promptResult = await client.prompt("echo", { message: "Hello from client prompt!" });
    console.log("Prompt result:", promptResult);
    
  } catch (error) {
    console.error("Error:", error);
  } finally {
    // 接続を閉じる
    await client.disconnect();
    console.log("Disconnected from server");
  }
}

main().catch(console.error);