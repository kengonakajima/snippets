import Groq from 'groq-sdk';
import dotenv from 'dotenv';

dotenv.config();

const groq = new Groq({
  apiKey: process.env.GROQ_API_KEY
});

async function main() {
  const chatCompletion = await groq.chat.completions.create({
    messages: [
      {
        role: "user",
        content: "Hello world! Please respond with a greeting."
      }
    ],
    model: "moonshotai/kimi-k2-instruct"
  });

  console.log(chatCompletion.choices[0]?.message?.content || "");
}

main().catch(console.error);