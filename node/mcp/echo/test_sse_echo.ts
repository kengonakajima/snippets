import { McpServer } from "@modelcontextprotocol/sdk/server/mcp.js";
import { SSEClientTransport } from "@modelcontextprotocol/sdk/client/sse.js";
import { Client as McpClient } from "@modelcontextprotocol/sdk/client/mcp.js";
import fetch from "node-fetch";

async function runTests() {
  console.log("Starting MCP echo server tests...");
  
  // Create MCP client with SSE transport
  const transport = new SSEClientTransport(new URL("http://localhost:3001/sse"), { requestInit: {} });
  const client = new McpClient({ 
    name: "EchoClient",
    version: "1.0.0" 
  });
  
  try {
    // Connect to the server
    console.log("Connecting to echo server...");
    await client.connect(transport);
    console.log("Connected successfully");
    
    // Test resource
    console.log("\nTesting resource...");
    const resourceResult = await client.readResource({ uri: "echo://hello-world" });
    console.log("Resource result:", resourceResult);
    
    // Test tool
    console.log("\nTesting tool...");
    const toolResult = await client.callTool({
      name: "echo",
      arguments: { message: "Hello from tool!" }
    });
    console.log("Tool result:", toolResult);
    
    // Test prompt
    console.log("\nTesting prompt...");
    const promptResult = await client.getPrompt({ 
      name: "echo", 
      arguments: { message: "Hello from prompt!" }
    });
    console.log("Prompt result:", promptResult);
    
    console.log("\nAll tests completed successfully!");
  } catch (error) {
    console.error("Test failed:", error);
  } finally {
    // Disconnect client
    await client.disconnect();
    console.log("Disconnected from server");
  }
}

// Make sure the server is running before starting tests
async function checkServer() {
  try {
    await fetch("http://localhost:3001/sse");
    console.log("Server is running, starting tests...");
    await runTests();
  } catch (error) {
    console.error("Error connecting to server:", error);
    console.log("Make sure to start the server with 'npm run start' before running tests");
  }
}

checkServer();