import express from "express";
import { McpServer } from "@modelcontextprotocol/sdk/server/mcp.js";
import { SSEServerTransport } from "@modelcontextprotocol/sdk/server/sse.js";
import { ResourceTemplate } from "@modelcontextprotocol/sdk/server/mcp.js";
import { z } from "zod";
const server = new McpServer({
    name: "Echo",
    version: "1.0.0"
});
server.resource("echo", new ResourceTemplate("echo://{message}", { list: undefined }), async (uri, { message }) => ({
    contents: [{
            uri: uri.href,
            text: `Resource echo: ${message}`
        }]
}));
server.tool("echo", { message: z.string() }, async ({ message }) => ({
    content: [{ type: "text", text: `Tool echo: ${message}` }]
}));
server.prompt("echo", { message: z.string() }, ({ message }) => ({
    messages: [{
            role: "user",
            content: {
                type: "text",
                text: `Please process this message: ${message}`
            }
        }]
}));
// ... set up server resources, tools, and prompts ...
const app = express();
// to support multiple simultaneous connections we have a lookup object from
// sessionId to transport
const transports = {};
app.get("/sse", async (_, res) => {
    const transport = new SSEServerTransport('/messages', res);
    transports[transport.sessionId] = transport;
    res.on("close", () => {
        delete transports[transport.sessionId];
    });
    await server.connect(transport);
});
app.post("/messages", async (req, res) => {
    const sessionId = req.query.sessionId;
    const transport = transports[sessionId];
    if (transport) {
        await transport.handlePostMessage(req, res);
    }
    else {
        res.status(400).send('No transport found for sessionId');
    }
});
app.listen(3001);
