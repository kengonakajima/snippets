const {
  plotArrayToImage,
  getAvg,
  getMin,
  getMax,
  calcAveragePower
}=require("./util.js")

// RLSアルゴリズムのパラメータ設定
const numWeights = 50;  // フィルタ係数の数
const lambda = 0.95;    // 忘却係数
const delta = 0.1;      // 正則化パラメータ

// 初期化
let weights = new Array(numWeights).fill(0);  // フィルタ係数の初期化
let inverseCorrelation = new Array(numWeights).fill(0).map(() => new Array(numWeights).fill(0));
for (let i = 0; i < numWeights; i++) {
  inverseCorrelation[i][i] = 1 / delta;
}

// 予測関数
function predict(input) {
//  console.log("input:",input);
  let output = 0;
  for (let i = 0; i < numWeights; i++) {
    output += weights[i] * (input[i]||0);
  }
  return output;
}

// 更新関数
function update(input, desiredOutput) {
  const output = predict(input);
  const error = desiredOutput - output;

  const gain = new Array(numWeights).fill(0);
  for (let i = 0; i < numWeights; i++) {
    let sum = 0;
    for (let j = 0; j < numWeights; j++) {
      sum += inverseCorrelation[i][j] * (input[j]||0);
    }
    gain[i] = sum / (lambda + input.reduce((acc, val, idx) => acc + val * sum, 0));
  }

  for (let i = 0; i < numWeights; i++) {
    weights[i] += gain[i] * error;
    for (let j = 0; j < numWeights; j++) {
      inverseCorrelation[i][j] -= gain[i] * gain[j] * lambda;
    }
  }
}


// RLSアルゴリズムの適用
function applyRLS(inputSignal, desiredOutput) {
  const outputSignal = [];
  const numSections = Math.floor(inputSignal.length / numWeights);

  for (let section = 0; section < numSections; section++) {
    const sectionStart = section * numWeights;
    const sectionEnd = sectionStart + numWeights;
    const sectionInput = inputSignal.slice(sectionStart, sectionEnd);
    const sectionDesiredOutput = desiredOutput.slice(sectionStart, sectionEnd);

    for (let i = 0; i < numWeights; i++) {
      const input = sectionInput.slice(0, i + 1);
      const output = predict(input);
      outputSignal.push(output);
      update(input, sectionDesiredOutput[i]);
    }
  }

  return outputSignal;
}


// 正弦波の生成
function generateSineWave(numSamples, frequency, amplitude) {
  const sineWave = [];
  for (let i = 0; i < numSamples; i++) {
    sineWave.push(amplitude * Math.sin(2 * Math.PI * frequency * i / numSamples));
  }
  return sineWave;
}

// ノイズの生成
function generateNoise(numSamples, noiseAmplitude) {
  const noise = [];
  for (let i = 0; i < numSamples; i++) {
    noise.push(noiseAmplitude * (Math.random() - 0.5));
  }
  return noise;
}

// デモンストレーション
function demonstrateRLS() {
  const numSamples = 1000;
  const sineWave = generateSineWave(numSamples, 50, 1);
  const noise = generateNoise(numSamples, 0.5);
  const noisySignal = sineWave.map((value, index) => value + noise[index]);
  const filteredSignal = applyRLS(noisySignal, sineWave);

  console.log('Noisy Signal:', noisySignal);
  console.log('Filtered Signal:', filteredSignal);

  plotArrayToImage([noisySignal,filteredSignal],1024,512,`plots/rls_demo.png`,1);
  
}

// デモンストレーションの実行
demonstrateRLS();
