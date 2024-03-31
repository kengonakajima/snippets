const mysql = require('mysql2');

// MySQL接続の設定
const config = {
  host: 'localhost',
  user: process.argv[2],
  password: process.argv[3],
  database: 'test',
};

// 2つの別々の接続を作成
const connectionA = mysql.createConnection(config);
const connectionB = mysql.createConnection(config);

// テーブルの作成とサンプルデータの挿入
connectionA.query(`
  CREATE TABLE IF NOT EXISTS users (
    id INT PRIMARY KEY,
    name VARCHAR(255)
  )
`);

connectionA.query(`
  INSERT INTO users (id, name)
  VALUES (1, 'Alice'), (2, 'Bob')
`);

// セッション1: トランザクションAを開始し、ユーザー1をロックする
function sessionA() {
  connectionA.beginTransaction((err) => {
    if (err) throw err;
    connectionA.query('SELECT * FROM users WHERE id = 1 FOR UPDATE', (err, result) => {
      if (err) {
        return connectionA.rollback(() => {
          throw err;
        });
      }
      console.log('Session A: Locked user 1');
      setTimeout(() => {
        connectionA.query('SELECT * FROM users WHERE id = 2 FOR UPDATE', (err, result) => {
          if (err) {
            return connectionA.rollback(() => {
              throw err;
            });
          }
          console.log('Session A: Locked user 2');
          connectionA.commit((err) => {
            if (err) {
              return connectionA.rollback(() => {
                throw err;
              });
            }
            console.log('Session A: Transaction committed');
          });
        });
      }, 1000); // 1秒のスリープ
    });
  });
}

// セッション2: トランザクションBを開始し、ユーザー2をロックする
function sessionB() {
  connectionB.beginTransaction((err) => {
    if (err) throw err;
    connectionB.query('SELECT * FROM users WHERE id = 2 FOR UPDATE', (err, result) => {
      if (err) {
        return connectionB.rollback(() => {
          throw err;
        });
      }
      console.log('Session B: Locked user 2');
      setTimeout(() => {
        connectionB.query('SELECT * FROM users WHERE id = 1 FOR UPDATE', (err, result) => {
          if (err) {
            return connectionB.rollback(() => {
              throw err;
            });
          }
          console.log('Session B: Locked user 1');
          connectionB.commit((err) => {
            if (err) {
              return connectionB.rollback(() => {
                throw err;
              });
            }
            console.log('Session B: Transaction committed');
          });
        });
      }, 1000); // 1秒のスリープ
    });
  });
}

// 2つのセッションを並行して実行
setTimeout(sessionA, 0);
setTimeout(sessionB, 500); // 0.5秒のディレイを追加
