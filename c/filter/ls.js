// サンプルデータ（入力値と出力値のペア）
const data = [
  { x: 1, y: 2 },
  { x: 2, y: 4 },
  { x: 3, y: 6 },
  { x: 4, y: 8 },
  { x: 5, y: 10 },
];

// 最小二乗法で直線の傾きと切片を計算する関数
function leastSquares(data) {
  const n = data.length;
  let sumX = 0;
  let sumY = 0;
  let sumXY = 0;
  let sumXX = 0;

  for (let i = 0; i < n; i++) {
    sumX += data[i].x;
    sumY += data[i].y;
    sumXY += data[i].x * data[i].y;
    sumXX += data[i].x * data[i].x;
  }

  const slope = (n * sumXY - sumX * sumY) / (n * sumXX - sumX * sumX);
  const intercept = (sumY - slope * sumX) / n;

  return { slope, intercept };
}

// 最小二乗法の実行
const result = leastSquares(data);
console.log(`傾き: ${result.slope}`);
console.log(`切片: ${result.intercept}`);

function do_ls(data) {
  const xysum=data.reduce((acc,val)=>{return acc+val.x*val.y} , 0);
  const xxsum=data.reduce((acc,val)=>{return acc+val.x*val.x} , 0);  
  const xsum=data.reduce((acc,val)=>{return acc+val.x} , 0);
  const ysum=data.reduce((acc,val)=>{return acc+val.y} , 0);
  const n=data.length;
  const xybar=xysum/n;
  const xxbar=xxsum/n;
  const xbar=xsum/n;
  const ybar=ysum/n;
  const a=(xybar - xbar * ybar) / (xxbar - xbar * xbar);
  const b=-a*xbar + ybar;

  console.log({xysum,xxsum,xsum,ysum,xybar,xxbar,xbar,ybar,a,b});
}

do_ls(data);

