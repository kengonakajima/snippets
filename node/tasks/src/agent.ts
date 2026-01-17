import OpenAI from "openai";
import { z } from "zod";

const client = new OpenAI({ apiKey: process.env.OPENAI_API_KEY });

export const PlanSchema = z.object({
  goal: z.string(),
  steps: z.array(
    z.object({
      id: z.string(),
      title: z.string(),
      detail: z.string().optional(),
      deps: z.array(z.string()).default([]),
      kind: z.string()
    })
  )
});

export type Plan = z.infer<typeof PlanSchema>;

const PlanJsonSchema = {
  type: "object",
  properties: {
    goal: { type: "string" },
    steps: {
      type: "array",
      items: {
        type: "object",
        properties: {
          id: { type: "string" },
          title: { type: "string" },
          detail: { type: "string" },
          deps: {
            type: "array",
            items: { type: "string" },
            default: []
          },
          kind: { type: "string" }
        },
        required: ["id", "title", "detail", "deps", "kind"],
        additionalProperties: false
      }
    }
  },
  required: ["goal", "steps"],
  additionalProperties: false
} as const;

type Artifact =
  | { type: "note"; title: string; content: string }
  | { type: "step_result"; stepId: string; note: string };

const artifacts: Artifact[] = [];
const logs: string[] = [];

export const tools: any = [
  {
    type: "function",
    function: {
      name: "writeNote",
      description: "中間成果物やメモを保存する",
      parameters: {
        type: "object",
        properties: {
          title: { type: "string" },
          content: { type: "string" }
        },
        required: ["title", "content"]
      }
    }
  },
  {
    type: "function",
    function: {
      name: "fetchUrl",
      description: "HTTP GET でテキストを取得する（デモ用の簡易版）",
      parameters: {
        type: "object",
        properties: {
          url: { type: "string" }
        },
        required: ["url"]
      }
    }
  }
];

async function callTool(name: string, args: Record<string, unknown>) {
  if (name === "writeNote") {
    const title = typeof args.title === "string" ? args.title : "";
    const content = typeof args.content === "string" ? args.content : "";
    if (!title || !content) {
      throw new Error("writeNote requires title and content");
    }
    artifacts.push({ type: "note", title, content });
    return { ok: true, id: String(artifacts.length) };
  }
  if (name === "fetchUrl") {
    const url = typeof args.url === "string" ? args.url : "";
    if (!url) {
      throw new Error("fetchUrl requires url");
    }
    const res = await fetch(url);
    const text = await res.text();
    return { status: res.status, text: text.slice(0, 4000) };
  }
  throw new Error(`Unknown tool ${name}`);
}

export async function makePlan(userGoal: string) {
  const schema = PlanSchema;
  const response: any = await client.responses.create({
    model: "gpt-5",
    input: [
      {
        role: "system",
        content: [
          {
            type: "input_text",
            text: "You are a planner agent. Output must follow the JSON schema."
          }
        ]
      },
      {
        role: "user",
        content: [
          {
            type: "input_text",
            text: `Goal: ${userGoal}\n制約: 各ステップは15分以内 / 外部ツールは必要最小限。`
          }
        ]
      }
    ],
    text: {
      format: {
        type: "json_schema",
        name: "Plan",
        schema: PlanJsonSchema,
        strict: true
      }
    }
  } as any);

  const payload = response.output?.[0]?.content?.[0];
  if (payload?.type !== "output_text") {
    throw new Error("No JSON plan returned");
  }
  const json = JSON.parse(payload.text);
  const plan = schema.parse(json);
  return plan;
}

export async function runPlan(plan: Plan) {
  const done = new Set<string>();

  while (done.size < plan.steps.length) {
    const next = plan.steps.find(step => !done.has(step.id) && step.deps.every(dep => done.has(dep)));
    if (!next) {
      throw new Error("Deadlock or unresolved dependencies");
    }

    const turn: any = await client.responses.create({
      model: "gpt-5",
      tools,
      tool_choice: "auto",
      input: [
        {
          role: "system",
          content: [
            {
              type: "input_text",
              text: "You are an execution agent. Think step-by-step. Prefer calling a tool when helpful."
            }
          ]
        },
        {
          role: "user",
          content: [
            {
              type: "input_text",
              text: `Step to execute: ${next.title}\nKind: ${next.kind}\nDetail: ${next.detail ?? ""}`
            },
            { type: "input_text", text: `Goal: ${plan.goal}` },
            {
              type: "input_text",
              text: `Artifacts so far: ${JSON.stringify(artifacts).slice(0, 2000)}`
            }
          ]
        }
      ]
    } as any);

    let finalThought = "";

    for (const item of turn.output ?? []) {
      const entry: any = item;
      if (entry.type === "message" && entry.role === "assistant") {
        const text = entry.content?.map((part: any) => (part.type === "output_text" ? part.text : "")).join("") ?? "";
        if (text) {
          logs.push(text);
        }
      }
      if (entry.type === "tool_call") {
        const result = await callTool(entry.name ?? "", entry.arguments ?? {});
        const follow: any = await client.responses.create({
          model: "gpt-5",
          input: [
            {
              role: "system",
              content: "Incorporate the tool result and decide if the step is complete."
            },
            {
              role: "assistant",
              content: [
                {
                  type: "tool_result",
                  tool_call_id: entry.call_id,
                  content: JSON.stringify(result)
                }
              ]
            }
          ]
        } as any);

        finalThought = follow.output
          ?.map((outputItem: any) =>
            outputItem.type === "message"
              ? outputItem.content?.map((part: any) => (part.type === "output_text" ? part.text : "")).join("") ?? ""
              : ""
          )
          .join("") ?? "";
      }
    }

    artifacts.push({ type: "step_result", stepId: next.id, note: finalThought });
    done.add(next.id);
  }

  return { artifacts, logs };
}
