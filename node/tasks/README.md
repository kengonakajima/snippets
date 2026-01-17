# Task Decomposition Agent

Node.js + TypeScript demo that follows the provided todo.txt design for a planner/executor agent. The CLI reads a goal from stdin (or argv), asks OpenAI to decompose it, then executes steps sequentially via tool calls.

## Setup

1. Install dependencies (pnpm preferred):
   ```bash
   pnpm install
   ```
2. Provide an OpenAI API key (Responses API with `gpt-5` access). The CLI reads `.env` automatically via `dotenv`, so you can store it like:
   ```bash
   echo "OPENAI_API_KEY=sk-..." > .env
   ```

## Usage

Generate a plan and run it in one shot:
```bash
echo "音声メモ→要約→Markdown整形" | pnpm dev
```

`pnpm dev` runs `tsx src/cli.ts`, so hot reload is available. For compiled output:
```bash
pnpm build
OPENAI_API_KEY=... node dist/cli.js <<'EOF'
リサーチ→構造化ノート→要約メール
EOF
```

## Notes

- Tools implemented: `writeNote` (stores artifacts in-memory) and `fetchUrl` (simple HTTP GET).
- The program keeps artifacts/logs in memory only; persistence can be swapped in later if needed.
- Errors from the API will bubble up; no extra retry logic is added per requirements.
