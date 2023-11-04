// 既存のconsole.logメソッドを保存
const originalConsoleLog = console.log;
console.log = function (...args) {
  const d=new Date();
  const timestamp = d.toISOString();
  originalConsoleLog(`[${timestamp}]`, ...args);
};

// テスト
console.log('Hello, world!');
