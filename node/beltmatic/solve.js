function generateExpressions(target, count, maxNum) {
  const operators = ['+', '-', '*', '/'];
  let n=0;
  while (n < count) {
    const nums = [];
    const ops = [];
    let numCount = Math.floor(Math.random() * 4) + 2;

    for (let i = 0; i < numCount; i++) {
      nums.push(Math.floor(Math.random() * maxNum) + 1);
    }

    for (let i = 0; i < numCount - 1; i++) {
      ops.push(operators[Math.floor(Math.random() * 4)]);
    }

    const expression = nums.reduce((acc, num, index) => {
      return acc + num + (ops[index] || '');
    }, '');

    if (eval(expression) === target) {
      console.log(target,"=",expression);
      n++;
    }
  }
}

// コマンドライン引数から問題を受け取る
const args = process.argv.slice(2);
const target = parseInt(args[0]);
const count = parseInt(args[1]) || 10;
const maxNum = parseInt(args[2]) || 100;

if (isNaN(target)) {
  console.error('please specify a number');
  process.exit(1);
}

generateExpressions(target, count, maxNum);
