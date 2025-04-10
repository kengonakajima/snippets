const {
  plotArrayToImage
} = require("./util.js");

// RLSアルゴリズムのパラメータ設定
const numWeights = 4; // フィルタ係数の数
const lambda = 0.98; // 忘却係数
const delta = 0.1; // 正則化パラメータ

// 初期化
let weights = Array(numWeights).fill(0); // フィルタ係数の初期化
let inverseCorrelation = Array(numWeights).fill(0).map(() => Array(numWeights).fill(0));
for (let i = 0; i < numWeights; i++) {
  inverseCorrelation[i][i] = 1 / delta;
}

// 予測関数
function predict(input) {
  let output = 0;
  for (let i = 0; i < input.length; i++) {
    output += weights[i] * input[i];
  }
  return output;
}

function update(input, desiredOutput) {
  const output = predict(input);
  const error = desiredOutput - output;
  const phi = input.slice().reverse();
  
  // ゲインの計算
  const phiTranspose = phi.map(val => [val]);
  const temp1 = multiplyMatrices(inverseCorrelation, phiTranspose);
  const temp2 = multiplyMatrixVector(phi, temp1);
  const temp3 = temp2.reduce((sum, val) => sum + val, lambda);
  const gain = temp1.map(row => row[0] / temp3);

  // 重みの更新
  weights = weights.map((weight, index) => weight + gain[index] * error);

  // 逆相関行列の更新
  const temp4 = outerProduct(gain, phi);
  inverseCorrelation = inverseCorrelation.map((row, i) => 
    row.map((val, j) => (val - temp4[i][j]) / lambda)
  );
}

// 行列とベクトルの乗算
function multiplyMatrixVector(matrix, vector) {
  return matrix.map(row => row.reduce((sum, val, i) => sum + val * vector[i], 0));
}

// 行列と行列の乗算
function multiplyMatrices(matrix1, matrix2) {
  return matrix1.map((row, i) =>
    matrix2[0].map((_, j) =>
      row.reduce((sum, val, k) => sum + val * matrix2[k][j], 0)
    )
  );
}

// 外積
function outerProduct(vector1, vector2) {
  return vector1.map(val1 => vector2.map(val2 => val1 * val2));
}



// RLSアルゴリズムの適用
function applyRLS(inputSignal, desiredOutput) {
  const outputSignal = [];
  for (let i = 0; i < inputSignal.length; i++) {
    const input = inputSignal.slice(i, i + numWeights);
    const output = predict(input);
    outputSignal.push(output);
    if (input.length === numWeights) {
      update(input, desiredOutput[i]);
    }
  }
  return outputSignal;
}

// 入力信号とデータ出力の生成
const inputSignal = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20];
const desiredOutput = inputSignal.map(val => 2 * val + 3);

// RLSアルゴリズムの適用
const outputSignal = applyRLS(inputSignal, desiredOutput);

console.log("Input Signal:", inputSignal);
console.log("Desired Output:", desiredOutput);
console.log("RLS Output:", outputSignal);

plotArrayToImage([inputSignal, desiredOutput, outputSignal], 1024, 512, `plots/rls_demo_minimal.png`, 1);
