import fetch from 'node-fetch';
import AbortController from 'abort-controller';

// タイムアウト付きのフェッチを作成
const fetchWithTimeout = (url, options = {}, timeout = 3000) => {
  const controller = new AbortController();
  const { signal } = controller;
  
  // タイムアウト用タイマー
  const timeoutId = setTimeout(() => controller.abort(), timeout);
  
  return fetch(url, { ...options, signal })
    .finally(() => clearTimeout(timeoutId));
};

// シンプルな HTTPクライアントを使用してテストする
async function runTests() {
  try {
    console.log("Starting MCP echo server tests...");
    
    // サーバーに接続
    const baseUrl = "http://localhost:3001";
    
    // 1. SSE接続テスト（接続のみ確認、実際には接続しない）
    console.log("Testing SSE endpoint availability...");
    const sseResponse = await fetchWithTimeout(`${baseUrl}/sse`, { 
      method: 'HEAD'
    }, 1000);
    
    if (sseResponse.ok) {
      console.log("SSE endpoint is available (response code:", sseResponse.status, ")");
      
      // 2. リソーステスト - セッションID付きのリクエスト
      console.log("\nTesting resource (echo)...");
      
      // まずSSE接続を開始して sessionID を取得
      console.log("Creating a temporary session ID...");
      const sessionId = `test-${Date.now()}`;
      
      // リソーステスト
      const resourceUrl = `${baseUrl}/messages?sessionId=${sessionId}&uri=echo://hello-world`;
      const resourceResponse = await fetchWithTimeout(resourceUrl, {}, 3000);
      
      if (resourceResponse.ok) {
        const resourceData = await resourceResponse.json();
        console.log("Resource test succeeded:", resourceData);
      } else {
        console.error("Resource test failed with status:", resourceResponse.status);
      }
      
      // 3. ツールテスト
      console.log("\nTesting tool (echo)...");
      const toolResponse = await fetchWithTimeout(`${baseUrl}/messages?sessionId=${sessionId}`, {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json'
        },
        body: JSON.stringify({
          jsonrpc: "2.0",
          id: 1,
          method: 'callTool',
          params: {
            name: 'echo',
            arguments: { message: 'Hello from tool!' }
          }
        })
      }, 3000);
      
      if (toolResponse.ok) {
        const toolData = await toolResponse.json();
        console.log("Tool test succeeded:", toolData);
      } else {
        console.error("Tool test failed with status:", toolResponse.status);
      }
      
      // 4. プロンプトテスト
      console.log("\nTesting prompt (echo)...");
      const promptResponse = await fetchWithTimeout(`${baseUrl}/messages?sessionId=${sessionId}`, {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json'
        },
        body: JSON.stringify({
          jsonrpc: "2.0",
          id: 2,
          method: 'getPrompt',
          params: {
            name: 'echo',
            arguments: { message: 'Hello from prompt!' }
          }
        })
      }, 3000);
      
      if (promptResponse.ok) {
        const promptData = await promptResponse.json();
        console.log("Prompt test succeeded:", promptData);
      } else {
        console.error("Prompt test failed with status:", promptResponse.status);
      }

      console.log("\nAll tests completed!");
    } else {
      console.error("Failed to establish SSE connection. Status:", sseResponse.status);
    }
  } catch (error) {
    console.error("Test failed:", error);
  }
}

// サーバーが実行中か確認してからテスト実行
async function checkServer() {
  try {
    // ヘッドリクエストでサーバーの動作確認
    const checkResponse = await fetch("http://localhost:3001/sse", { 
      method: 'HEAD',
      timeout: 1000
    });
    
    if (checkResponse.ok) {
      console.log("Server is running, starting tests...");
      await runTests();
    } else {
      console.error("Server returned an error status:", checkResponse.status);
      console.log("Make sure the server is running properly with 'npm run start'");
    }
  } catch (error) {
    console.error("Error connecting to server:", error);
    console.log("Make sure to start the server with 'npm run start' before running tests");
  }
}

checkServer();