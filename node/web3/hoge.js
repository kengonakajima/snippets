

// $ geth --http --http.corsdomain "*" --http.vhosts "*" --http.api "web3,eth,net" --http.authkey "/path/to/your/auth.key"

const jwt = require('jsonwebtoken');

const Web3 = require('web3');
//const providerUrl = 'http://localhost:8545'; // Ethereumノードに接続するエンドポイント
const providerUrl="https://ethereum.rpc.thirdweb.com";

const fs = require('fs');
const filePath = '/tmp/jwtsecret';
const jwtSecret  = fs.readFileSync(filePath, 'utf-8').split('\n')[0];


const addr=process.argv[2];
if(!addr) {
  console.log("invalid arg: need addr");
  process.exit(1);
}

const payload = {
  sub: 'kengonakajima', // ユーザー名
  iat: Math.floor(Date.now() / 1000), // トークンの発行時間
  exp: Math.floor(Date.now() / 1000) + 60 * 60 // トークンの有効期限（1時間）
};

// JWTトークンを生成
const jwtToken = jwt.sign(payload, jwtSecret);
console.log("jwtToken:",jwtToken);


// HTTPプロバイダーを設定
const httpProvider = new Web3.providers.HttpProvider(providerUrl, {
  headers: [
    {
      name: 'Authorization',
      value: `Bearer ${jwtToken}`
    }
  ]
});



const web3 = new Web3(httpProvider);

async function getEthBalance(address) {
  const balanceWei = await web3.eth.getBalance(address);
  const balanceEth = web3.utils.fromWei(balanceWei, 'ether');
  return balanceEth;
}



getEthBalance(addr).then((balance) => {
  console.log(`Address has a balance of ${balance} ETH`);
}).catch((error) => {
  console.error(error);
});
