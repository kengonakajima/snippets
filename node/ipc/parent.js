// 親プロセス側: 子プロセスに素数リストを依頼するサンプル
const { fork } = require("child_process");
const path = require("path");

const child = fork(path.join(__dirname, "child.js"));
let nextId = 1;

function listPrimes(from, to) {
  return new Promise((resolve) => {
    const id = nextId++;
    const handleMessage = (msg) => {
      if (!msg || msg.type !== "result" || msg.id !== id) return;
      child.off("message", handleMessage);
      resolve(msg.payload);
    };
    child.on("message", handleMessage);
    child.send({ type: "listPrimes", id, payload: { from, to } });
  });
}

if (require.main === module) {
  listPrimes(2, 50).then((primes) => {
    console.log(primes.join(", "));
    child.disconnect();
    child.kill();
  });
}

module.exports = { listPrimes };
