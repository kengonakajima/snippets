// RLSをゼロから実装する。

const {
  plotArrayToImage,
  createMatrix,
  transposeMatrix,
  multiplyMatrices,
  invertMatrix,
  divideMatrixByScalar,
  subtractMatrices,
  multiplyMatrixByScalar,
  addMatrices,
}=require("./util.js")


// d次の行ベクトルの入力 a
// d次の行ベクトルの出力 b (正解ラベルデータ)
// dxd行列のP
// https://manabitimes.jp/math/2084
// 重回帰RLSの更新式を導出している。
class RLSManabi {
  constructor(d) {
    this.d=d;
    this.theta = new Array(d).fill(0);
    this.P = createMatrix(d,1);
  }

  predict(x) {
    console.log("predict: x:",x,"theta:",this.theta);
    let sum=0;
    for(let i=0;i<this.theta.length;i++) {
      sum+=x[i]*this.theta[i];
    }
    return sum;
  }


  // P[n+1] = P[n] - ( P[n] * a_[n+1] * a_[n+1]^T * P[n] ) / ( 1 + a_[n+1]^T * P[n] * a_[n+1] )
  //                   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  //                          分子         Q : n x n                    分母      r : scalar
  // theta[n+1] = theta[n] + P[n+1] * a_[n+1] * (b[n+1] - a_[n+1]^T * theta[n] )
  //                                                        ~~~~~~~~~~~~~~~~~~
  //                                                                 s0 : scalar
  //                                            ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  //                                                    s : scalar
  
  update(x, y) {
    const a_n1=[x];
    const a_n1_T=transposeMatrix(a_n1);
    console.log({a_n1,a_n1_T});

    const P_n=this.P;
    console.log({P_n});
    const r0=multiplyMatrices(P_n,a_n1_T);
    console.log({r0});
    const r1=multiplyMatrices(a_n1,r0);
    const r=1+r1[0][0]; // r1: [[1]]
    console.log({r1,r});
    const q0=multiplyMatrices(a_n1,P_n);
    console.log({q0});
    const q1=multiplyMatrices(a_n1_T,q0);
    console.log({q1});
    const Q=multiplyMatrices(P_n,q1);
    console.log({Q});
    const Q_divided_by_r=divideMatrixByScalar(Q,r);
    console.log({Q_divided_by_r});
    const P_n1=subtractMatrices(P_n,Q_divided_by_r); // P[n+1]
    console.log({P_n1});
    const theta_n=[this.theta];
    const s0=multiplyMatrices(theta_n,a_n1_T)[0][0]; // s[[1]]
    console.log({s0});
    const b_n1=y;
    const s=b_n1-s0;
    console.log({s});
    const t0=multiplyMatrices(a_n1,P_n1); 
    console.log({t0});
    const t1=multiplyMatrixByScalar(t0,s);
    console.log({t1});
    const theta_n1=addMatrices(t1,theta_n);

    /*
      t0とt1が、ずっと同じ
      { t0: [ [ 0.5, 0.5 ] ] }
      { t1: [ [ 1, 1 ] ] }
      こういう値をとり続ける。そのせいで thetaも結局そうなる。
      Pの値もずっと4要素とも同じになる。
      Qもそうなっている。
      q0もそうなっている。
      aと a^Tをかけてどうなるのか
      a: [0,1]
      */

    this.P=P_n1;
    this.theta=theta_n1[0];
  }
  init(data) {
    // w = (X^T * X)^-1 * X^T * y
    // Xは、xが全部並んでいるデザイン行列. 各行が各データを表しており、各列が各入力変数を表す。
    // デザイン行列Xを作る
    const N=data.length; // データの列の長さ
    const M=data[0].x.length; // データの行の長さ
    const X=[]; // 行列は2次元配列(ベクトルの配列)
    const Y=[]; // 1次元配列、スカラ値の配列
    for(let i=0;i<N;i++) {
      X.push(data[i].x);
      Y.push([data[i].y]);  
    }
  
    
    // X^Tを作成する
    const X_T=transposeMatrix(X);
    const P=multiplyMatrices(X_T,X); // P = X^T * X
    console.log("X:",X,"X^T:",X_T);
    console.log("P:",P);
    const invP=invertMatrix(P);
    console.log("invP:",invP);
    const Q=multiplyMatrices(invP,X_T); // Q = P^-1 * X^T
    console.log("Q:",Q);
    const w_=multiplyMatrices(Q,Y); // これが偏回帰係数
    console.log("w_:",w_); // w_: [ [ 0 ], [ 1.9999999999999998 ] こういうの
    const w=[];
    for(const m of w_) w.push(m[0]);
    console.log("w:",w); // w: [ 0, 1.9999999999999998 ]

    this.theta=w;
    this.P=P;
    
  }
}

// rls0.js (清水版)の重回帰バージョン
// https://yuyumoyuyu.com/2020/12/27/recursiveleastsquares/
class RLSShimizu {
  constructor(d) {
    this.d=d;
    this.theta=[];
    for(let i=0;i<d;i++) this.theta.push([0]); // rls1.jsではd次の行ベクトルだったが、清水版ではthetaは列ベクトルである
    this.P = createMatrix(d);
    this.lambda=0.2;
  }
  predict(x) {
    console.log("predict: x:",x,"theta:",this.theta);
    let sum=0;
    for(let i=0;i<this.theta.length;i++) {
      sum+=x[i]*this.theta[i];
    }
    return sum;
  }
  /*
    P[n] = P[n-1] / ( lambda + x(n) * P[n-1] * x(n)^T )
                     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                       分母 s : scalar

    gain = ( P[n-1] * x(n)^T ) / ( lambda + x(n) * P[n-1] * x(n)^T )
            ~~~~~~~~~~~~~~~~     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
              分子 g0                分母 s
              
    theta[n] = theta[n-1] + gain * ( y(n) - x(n) * theta[n-1] )
                                            ~~~~~~~~~~~~~~~~~~~
                                              t0:  1xd * dx1 -> scalar
                                    ~~~~~~~~~~~~~~~~~~~~~~~~~~
                                        t1 : scalar
                            ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                               t2 : 列ベクトル
   */
  
  update(x,y) {
    const x_n=[x];
    const x_n_t=transposeMatrix(x_n);
    const s0=multiplyMatrices(x_n,this.P);
    const s1=multiplyMatrices(s0,x_n_t);
    const s=this.lambda + s1[0][0]; // scalar
    const P_n=multiplyMatrixByScalar(this.P, 1/s); // dxd行列
    const g0=multiplyMatrices(this.P, x_n_t); // 列ベクトル
    const gain=multiplyMatrixByScalar(g0, 1/s); // 列ベクトル
    const t0 = multiplyMatrices(x_n,this.theta); // scalar
    const t1 = y-t0[0][0];
    const t2 = multiplyMatrixByScalar(gain,t1);
    const theta_n = addMatrices(this.theta,t2);
    console.log({x_n,x_n_t,s0,s1,s,P_n,g0,gain,t0,theta: this.theta, t1, t2, theta_n});
    this.theta=theta_n;
    this.P=P_n;
  }

  init(data) {
    // w = (X^T * X)^-1 * X^T * y
    // Xは、xが全部並んでいるデザイン行列. 各行が各データを表しており、各列が各入力変数を表す。
    // デザイン行列Xを作る
    const N=data.length; // データの列の長さ
    const M=data[0].x.length; // データの行の長さ
    const X=[]; // 行列は2次元配列(ベクトルの配列)
    const Y=[]; // 1次元配列、スカラ値の配列
    for(let i=0;i<N;i++) {
      X.push(data[i].x);
      Y.push([data[i].y]);  
    }
  
    
    // X^Tを作成する
    const X_T=transposeMatrix(X);
    const P=multiplyMatrices(X_T,X); // P = X^T * X
    const invP=invertMatrix(P);
    const Q=multiplyMatrices(invP,X_T); // Q = P^-1 * X^T
    const w=multiplyMatrices(Q,Y); // これが偏回帰係数  w: [ [ 0 ], [ 1.9999999999999998 ] こういうの
    console.log({N,M,X,Y,X_T,P,invP,Q,w}); 
    this.theta=w;
    this.P=P;
  }
  

}



// データセット ( 0番目+1番目*2)
const k=0; // 切片には対応してない
const initData = [
  { x: [3,1], y: 2+k },
  { x: [0,1], y: 2+k },
  { x: [1,4], y: 8+k },  
];
const onlineData = [
  { x: [0,1], y: 2+k },
  { x: [2,2], y: 4+k },
  { x: [1,4], y: 8+k },
  { x: [-1,2], y: 4+k },
  { x: [-1,2], y: 4+k },
  // きりかわり
  { x: [0,1], y: 0+k },
  { x: [2,2], y: 4+k },
  { x: [1,4], y: 2+k },
  { x: [-1,2], y: -2+k },
  { x: [-1,2], y: -2+k },
  { x: [3,2], y: 6+k },
  { x: [1,4], y: 2+k },
  { x: [-1,2], y: -2+k },
  { x: [-1,4], y: -2+k },
  { x: [7,4], y: 14+k },
  { x: [4,4], y: 8+k },
  { x: [4,4], y: 8+k },
  { x: [4,4], y: 8+k },
  { x: [4,0], y: 8+k },
  { x: [4,0], y: 8+k },
  { x: [4,0], y: 8+k },
  { x: [4,0], y: 8+k },
  { x: [4,0], y: 8+k },
  { x: [4,0], y: 8+k },
  { x: [4,0], y: 8+k },          
];


const rls0=new RLSManabi(2);
rls0.init(initData);

const rls1=new RLSShimizu(2);
rls1.init(initData);


// 学習
onlineData.forEach(({ x, y }) => {
  console.log("Manabi Predicting..");
  const prediction0 = rls0.predict(x); // 現在のパラメータを使用して予測値を計算
  console.log(`Manabi Input: [${x}], Prediction: ${prediction0}, Actual: ${y}`); // 入力、予測値、実際の値を表示
  console.log("Manabi Updating..");  
  rls0.update(x, y); // パラメータの更新

  console.log("Shimizu Predicting..");
  const prediction1 = rls1.predict(x); // 現在のパラメータを使用して予測値を計算
  console.log(`Shimizu Input: [${x}], Prediction: ${prediction0}, Actual: ${y}`); // 入力、予測値、実際の値を表示
  console.log("Shimizu Updating..");  
  rls1.update(x, y); // パラメータの更新

  const diff0=y-prediction0;
  const diff1=y-prediction1;
  console.log("DIFF:",(prediction1-prediction0),"prediction0:",prediction0,"prediction1:",prediction1,"diff0:",diff0,"diff1:",diff1);

  // shimizu版はlambdaで調整できる! 
  
});

