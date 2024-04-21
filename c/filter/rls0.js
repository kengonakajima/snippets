// RLSをゼロから構築する(単回帰)

class RLS {
  constructor(lambda = 0.99) {
    this.lambda = lambda; // 忘却係数を設定
    this.theta = 0;
    this.p = 1;
  }

  predict(x) {
    return x * this.theta;
  }

  // theta: T
  // lambda: l
  // L :
  // | l^0, ......, 0 |
  // | 0, l^1, ..., 0 |
  // | 0,.....  , l^n |  
  // T[n] = T[n-1] + gain * ( y(n) - T[n-1]*x(n) );
  // gain = ( p[n-1] * x(n) ) / ( l + p[n-1] * (x(n)^2) )
  // p[n] = 1 / ( X[n]^T * L * X[n] )  右半分はsummationで表現できるスカラ値。  
  // r = X[n]^T * L * X[n] ) = sum(i=0 to n, l^(n-i) * x(i) * x(i) )  xはX[n]の要素
  // p[n] = 1 / r  だが、rが0になることがある。ので、計算できない。最初Infになる
  // ただし、以下の式だと、分母は0にならあい。
  // p[n] = p[n-1] / ( l + p[n-1] * x(n) * x(n)) 
  
  update(x, y) {
    const new_p = this.p / ( this.lambda + this.p  * x * x)
    const gain = ( this.p * x ) / ( this.lambda + this.p  * x * x)
    const new_theta = this.theta + gain * (y-this.theta*x);
    console.log("update:",{x,y,cur_p:this.p,cur_theta:this.theta,new_p,gain,new_theta});
    this.p=new_p;
    this.theta=new_theta;
  }
}



// データセット ( 0番目+1番目*2)
const k=0; // 切片には対応してない
const data = [
  { x: 1, y: 2+k },
  { x: 2, y: 4+k },
  { x: 4, y: 8+k },
  { x: -1, y: -2+k },
  { x: 1, y: 2+k },
  { x: 12, y: 24+k },
  { x: 5, y: 10+k },
  { x: 4, y: 8+k },
  { x: 3, y: 6+k },
  { x: 0, y: 0+k },
  { x: 2, y: 4+k },          
];


const rls=new RLS();

// 学習
data.forEach(({ x, y }) => {
  console.log("Predicting..");
  const prediction = rls.predict(x); // 現在のパラメータを使用して予測値を計算
  console.log(`Input: [${x}], Prediction: ${prediction}, Actual: ${y}`); // 入力、予測値、実際の値を表示
  console.log("Updating..");  
  rls.update(x, y); // パラメータの更新
  console.log("theta:",rls.theta);
});

