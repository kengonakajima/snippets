// RLSをゼロから実装する。
// https://manabitimes.jp/math/2084
// 重回帰RLSの更新式を導出している。
const {
  plotArrayToImage,
}=require("./util.js")




function transposeMatrix(matrix) {
  const rows = matrix.length;
  const cols = matrix[0].length;
  
  const out = [];
  
  for (let j = 0; j < cols; j++) {
    out[j] = [];
    for (let i = 0; i < rows; i++) {
      out[j][i] = matrix[i][j];
    }
  }
  
  return out;
}

function multiplyMatrices(matrix1, matrix2) {
  const rows1 = matrix1.length;
  const cols1 = matrix1[0].length;
  const rows2 = matrix2.length;
  const cols2 = matrix2[0].length;

  if (cols1 !== rows2) {
    throw new Error("Cannot multiply matrices: dimensions do not match.");
  }

  const resultMatrix = [];

  for (let i = 0; i < rows1; i++) {
    resultMatrix[i] = [];
    for (let j = 0; j < cols2; j++) {
      let sum = 0;
      for (let k = 0; k < cols1; k++) {
        sum += matrix1[i][k] * matrix2[k][j];
      }
      resultMatrix[i][j] = sum;
    }
  }

  return resultMatrix;
}

function invertMatrix(matrix) {
  const n = matrix.length;

  // 単位行列を作成
  const identity = Array(n)
    .fill()
    .map(() => Array(n).fill(0));
  for (let i = 0; i < n; i++) {
    identity[i][i] = 1;
  }

  // 行列とその単位行列を拡張して、ガウス・ジョルダン法を適用
  const augmented = matrix.map((row, i) => [...row, ...identity[i]]);

  // ガウス・ジョルダン法
  for (let i = 0; i < n; i++) {
    // ピボット要素を1にする
    const pivot = augmented[i][i];
    for (let j = 0; j < 2 * n; j++) {
      augmented[i][j] /= pivot;
    }

    // ピボット要素以外の要素を0にする
    for (let k = 0; k < n; k++) {
      if (k !== i) {
        const factor = augmented[k][i];
        for (let j = 0; j < 2 * n; j++) {
          augmented[k][j] -= factor * augmented[i][j];
        }
      }
    }
  }

  // 拡張行列から逆行列を抽出
  const inverse = augmented.map(row => row.slice(n));

  return inverse;
}

function addMatrices(matrix1, matrix2) {
  // 行列のサイズが等しいかチェック
  if (matrix1.length !== matrix2.length || matrix1[0].length !== matrix2[0].length) {
    throw new Error("行列のサイズが異なります");
  }

  const result = [];
  for (let i = 0; i < matrix1.length; i++) {
    result[i] = [];
    for (let j = 0; j < matrix1[0].length; j++) {
      result[i][j] = matrix1[i][j] + matrix2[i][j];
    }
  }
  return result;
}

function subtractMatrices(matrix1, matrix2) {
  // 行列のサイズが等しいかチェック
  if (matrix1.length !== matrix2.length || matrix1[0].length !== matrix2[0].length) {
    throw new Error("行列のサイズが異なります");
  }

  const result = [];
  for (let i = 0; i < matrix1.length; i++) {
    result[i] = [];
    for (let j = 0; j < matrix1[0].length; j++) {
      result[i][j] = matrix1[i][j] - matrix2[i][j];
    }
  }
  return result;
}

function createMatrix(d,val) {
  const o=[];
  for(let i=0;i<d;i++) o.push(new Array(d).fill(val));
  return o;
}
function dotProduct(vector1, vector2) {
  // ベクトルの長さが等しいかチェック
  if (vector1.length !== vector2.length) {
    throw new Error("ベクトルの長さが異なります");
  }

  // 内積の計算
  let result = 0;
  for (let i = 0; i < vector1.length; i++) {
    result += vector1[i] * vector2[i];
  }

  return result;
}

function divideMatrixByScalar(matrix, scalar) {
  if (scalar === 0) {
    throw new Error("スカラー値が0です。ゼロ除算は許可されていません。");
  }

  const result = [];
  for (let i = 0; i < matrix.length; i++) {
    result[i] = [];
    for (let j = 0; j < matrix[0].length; j++) {
      result[i][j] = matrix[i][j] / scalar;
    }
  }
  return result;
}

function multiplyMatrixByScalar(matrix, scalar) {
  const result = [];
  for (let i = 0; i < matrix.length; i++) {
    result[i] = [];
    for (let j = 0; j < matrix[0].length; j++) {
      result[i][j] = matrix[i][j] * scalar;
    }
  }
  return result;
}

// d次の行ベクトルの入力 a
// d次の行ベクトルの出力 b (正解ラベルデータ)
// dxd行列のP 
class RLS {
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


const rls=new RLS(2);
rls.init(initData);



// 学習
onlineData.forEach(({ x, y }) => {
  console.log("Predicting..");
  const prediction = rls.predict(x); // 現在のパラメータを使用して予測値を計算
  console.log(`Input: [${x}], Prediction: ${prediction}, Actual: ${y}`); // 入力、予測値、実際の値を表示
  console.log("Updating..");  
  rls.update(x, y); // パラメータの更新
  console.log("theta:",rls.theta);
});

