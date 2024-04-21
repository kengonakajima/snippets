// multiple regression
// 重回帰のサンプル



function transposeMatrix(matrix) {
  const rows = matrix.length;
  const cols = matrix[0].length;
  
  const transposedMatrix = [];
  
  for (let j = 0; j < cols; j++) {
    transposedMatrix[j] = [];
    for (let i = 0; i < rows; i++) {
      transposedMatrix[j][i] = matrix[i][j];
    }
  }
  
  return transposedMatrix;
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

function mr() {
  const data = [
    { x: [1,2], y: 4 },
    { x: [2,3], y: 6 },
    { x: [0,3], y: 6 },
    { x: [4,1], y: 2 },
  ];
  
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
  const w=[];1
  for(const m of w_) w.push(m[0]);
  console.log("w:",w); // w: [ 0, 1.9999999999999998 ]

  const testData = [2,8]; 
  const dot=dotProduct(w,testData);
  console.log("dot:",dot); // dot: 15.999999999999998

}



mr();
