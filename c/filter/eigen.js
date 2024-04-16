const numeric=require("numeric")

// 入力信号のサンプルデータ（512要素の1次元Float配列）
const inputSignal = [
  0.1, 0.2, 0.3,0.4,
  0.1, 0.2, 0.3,0.4,
  0.1, 0.2, 0.3,0.4,
  0.1, 0.2, 0.3,0.4,
  
];
for(let i=0;i<inputSignal.length;i++) {
  inputSignal[i]*=2;
}



// 相関行列を計算する関数
function calculateCorrelationMatrix(inputSignal) {
  const N = inputSignal.length;
  const correlationMatrix = numeric.rep([N, N], 0);

  for (let i = 0; i < N; i++) {
    for (let j = 0; j < N; j++) {
      correlationMatrix[i][j] = inputSignal[i] * inputSignal[j];
    }
  }

  return correlationMatrix;
}

// 最大固有値を計算する関数
function calculateMaxEigenvalue(correlationMatrix) {
  const eigenvalues = numeric.eig(correlationMatrix).lambda.x;
  const maxEigenvalue = Math.max(...eigenvalues);
  return maxEigenvalue;
}

// 相関行列の計算
const correlationMatrix = calculateCorrelationMatrix(inputSignal);
//console.log('Correlation Matrix:');
//console.log(correlationMatrix);

// 最大固有値の計算
const maxEigenvalue = calculateMaxEigenvalue(correlationMatrix);
console.log('Max Eigenvalue:', maxEigenvalue);




// 自己相関関数を計算する関数
function calculateAutocorrelation(inputSignal) {
  const N = inputSignal.length;
  const autocorrelation = numeric.rep([N], 0);

  for (let i = 0; i < N; i++) {
    for (let j = 0; j < N - i; j++) {
      autocorrelation[i] += inputSignal[j] * inputSignal[j + i];
    }
    autocorrelation[i] /= (N - i);
  }

  return autocorrelation;
}

// 最大値を計算する関数
function calculateMax(autocorrelation) {
  const max = Math.max(...autocorrelation);
  return max;
}

// 自己相関関数の計算
const autocorrelation = calculateAutocorrelation(inputSignal);
console.log('Autocorrelation:');
console.log(autocorrelation);

// 最大値の計算
const max = calculateMax(autocorrelation);
console.log('corel Max:', max);
