// 子プロセス側: 親から受け取った範囲で素数を計算して返す
function isPrime(n) {
  if (n < 2) return false;
  if (n === 2) return true;
  if (n % 2 === 0) return false;
  const limit = Math.floor(Math.sqrt(n));
  for (let i = 3; i <= limit; i += 2) {
    if (n % i === 0) return false;
  }
  return true;
}

function listPrimes(from, to) {
  const primes = [];
  for (let n = from; n <= to; n++) {
    if (isPrime(n)) primes.push(n);
  }
  return primes;
}

process.on("message", (msg) => {
  if (!msg || msg.type !== "listPrimes") return;
  const { from, to } = msg.payload;
  const result = listPrimes(from, to);
  process.send({ type: "result", id: msg.id, payload: result });
});
