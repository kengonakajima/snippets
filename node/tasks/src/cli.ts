import "dotenv/config";
import { makePlan, runPlan } from "./agent.js";

async function readAllInput() {
  const chunks: Buffer[] = [];
  for await (const chunk of process.stdin) {
    chunks.push(Buffer.isBuffer(chunk) ? chunk : Buffer.from(chunk));
  }
  return Buffer.concat(chunks).toString("utf8");
}

async function main() {
  const goalInput = (await readAllInput()).trim();
  const goal = goalInput || process.argv.slice(2).join(" ");
  if (!goal) {
    console.error("Usage: echo 'your goal' | pnpm dev");
    process.exit(1);
  }

  if (!process.env.OPENAI_API_KEY) {
    console.error("OPENAI_API_KEY is required in the environment");
    process.exit(1);
  }

  console.log(`# Goal\n${goal}`);

  const plan = await makePlan(goal);
  console.log("\n# Plan");
  console.log(JSON.stringify(plan, null, 2));

  const { artifacts, logs } = await runPlan(plan);
  console.log("\n# Logs");
  logs.forEach(entry => {
    console.log(entry);
  });

  console.log("\n# Artifacts");
  console.log(JSON.stringify(artifacts, null, 2));
}

main();
