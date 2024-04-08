function matchedFilter(inputSignal, referenceSignal) {
  const inputLength = inputSignal.length;
  const referenceLength = referenceSignal.length;
  const outputLength = inputLength + referenceLength - 1;
  const output = new Array(outputLength).fill(0);

  // クロス相関の計算
  for (let i = 0; i < inputLength; i++) {
    for (let j = 0; j < referenceLength; j++) {
      output[i + j] += inputSignal[i] * referenceSignal[j];
    }
  }

  // ピークの検出
  let peakIndex = 0;
  let peakValue = output[0];
  for (let i = 1; i < outputLength; i++) {
    if (output[i] > peakValue) {
      peakValue = output[i];
      peakIndex = i;
    }
  }

  // 遅延の推定
  const delay = peakIndex - referenceLength + 1;

  return delay;
}

// 使用例
//const inputSignal = [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0];
const inputSignal = [0,0,0,1, 2,1,0,0, 0,0,2,3,5,3,1,2,1,0,0,0,0];
//const referenceSignal = [0.2, 0.4, 0.6, 0.8];
const referenceSignal = [2,3,5,3];
const estimatedDelay = matchedFilter(inputSignal, referenceSignal);
console.log("Estimated delay:", estimatedDelay);
