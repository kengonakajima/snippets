const {
  plotArrayToImage,
  getAvg,
  getMin,
  getMax,
  calcAveragePower
}=require("./util.js")


// LMSアルゴリズムのパラメータ設定
const numWeights = 10;  // フィルタ係数の数. 10でもそれなりに動く。50ぐらいにするとなめらかなサイン波が現れる
const mu = 0.01;        // ステップサイズを大きくすると、速く収束するが、大きなギザギザが残ってしまう。

// 初期化
let weights = new Array(numWeights).fill(0);  // フィルタ係数の初期化

// 予測関数
function predict(input) {
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

  for (let i = 0; i < numWeights; i++) {
    weights[i] += mu * error * (input[i]||0);
  }
}

// LMSアルゴリズムの適用
function applyLMS(inputSignal, desiredOutput) {
  const outputSignal = [];
  for (let i = 0; i < inputSignal.length; i++) {
    const input = inputSignal.slice(Math.max(0, i - numWeights + 1), i + 1);
    const output = predict(input);
    outputSignal.push(output);
    update(input, desiredOutput[i]);
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
function demonstrateLMS() {
  const numSamples = 1000;
  const sineWave = generateSineWave(numSamples, 20, 0.5); // 振幅を小さくすると weightsの値も小さくなる。　周期を高くすると、weightsの値は大きくなる。
  const noise = generateNoise(numSamples, 0.5);
  const noisySignal = sineWave.map((value, index) => value + noise[index]);
  const filteredSignal = applyLMS(noisySignal, sineWave);
  const avgPw=calcAveragePower(sineWave);
  console.log('Original Signal:',sineWave);
  console.log('Noisy Signal:', noisySignal);
  console.log('Filtered Signal:', filteredSignal);
  console.log("averagePower:",avgPw); // 0.12ぐらいだった(振幅0.5, 周期による変化はない)

  plotArrayToImage([sineWave,noisySignal],1024,512,`plots/lms_demo.png`,1);
  plotArrayToImage([filteredSignal,weights],1024,512,`plots/lms_demo_w.png`,1);  
  console.log("weights:",weights.join("\n"))
}

// デモンストレーションの実行
demonstrateLMS();
