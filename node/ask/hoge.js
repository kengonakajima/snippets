const fs = require('fs');

// 環境変数からパスワードを取得
const password = process.env.MY_PASSWORD;

if (!password) {
  console.error('Error: MY_PASSWORD environment variable is not set');
  process.exit(1);
}

// パスワードをp.txtに保存
fs.writeFileSync('p.txt', password, 'utf8');

console.log('Password has been saved to p.txt');
