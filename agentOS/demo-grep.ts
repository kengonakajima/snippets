// agentOS demo: VM内でgrepを実行（計測付き）
import { AgentOs } from "@rivet-dev/agent-os-core";
import { allowAll } from "@secure-exec/core";
import common from "@rivet-dev/agent-os-common";

const t0 = performance.now();
const vm = await AgentOs.create({ software: common, permissions: allowAll });
const t1 = performance.now();
console.log(`VM作成: ${(t1 - t0).toFixed(0)}ms\n`);

async function timed(label: string, cmd: string) {
  const start = performance.now();
  const r = await vm.exec(cmd);
  const ms = (performance.now() - start).toFixed(0);
  console.log(`[${ms}ms] ${label}`);
  console.log(r.stdout);
}

await timed("printf | grep ap", 'printf "apple\\nbanana\\ncherry\\navocado\\nblueberry\\napricot\\ngrape\\n" | grep ap');
await timed("grep '[0-9]'", 'printf "hello\\ntest123\\nworld\\n42dogs\\n" | grep "[0-9]"');
await timed("grep -v 'a'", 'printf "apple\\nbanana\\ncherry\\ngrape\\n" | grep -v a');
await timed("grep -c 'a'", 'printf "apple\\nbanana\\ncherry\\ngrape\\n" | grep -c a');
await timed("sort | uniq", 'printf "banana\\napple\\nbanana\\ncherry\\napple\\n" | sort | uniq');

await vm.dispose();
console.log(`合計: ${(performance.now() - t0).toFixed(0)}ms`);
