// rls0.js (清水版)の重回帰バージョン

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

// 正方行列を作る
function createMatrix(d) {
  const o=[];
  for(let i=0;i<d;i++) {
    o[i]=[];
    for(let j=0;j<d;j++) {
      let v=0;
      if(j==i)v=1;
      o[i][j]=v;
    }
  }
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

class RLSShimizu {
  constructor(d) {
    this.d=d;
    this.theta=[];
    for(let i=0;i<d;i++) this.theta.push([0]); // rls1.jsではd次の行ベクトルだったが、清水版ではthetaは列ベクトルである
    this.P = createMatrix(d);
    this.lambda=0.99;
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


const rls=new RLSShimizu(2);
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



