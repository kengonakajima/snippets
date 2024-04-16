const {
  plotArrayToImage,
  getAvg,
  getMin,
  getMax,
  calcAveragePower
}=require("./util.js")


// LMSアルゴリズムのパラメータ設定
const numWeights = 10;  // フィルタ係数の数. 10でもそれなりに動く。50ぐらいにするとなめらかなサイン波が現れる


// 初期化(LMS,NLMS共通)
let weights = new Array(numWeights).fill(0);  // フィルタ係数の初期化

// 予測関数(LMS,NLMS共通)
function predict(input) {
  let output = 0;
  for (let i = 0; i < numWeights; i++) {
    output += weights[i] * (input[i]||0);
  }
  return output;
}

// LMSの更新関数
function updateLMS(input, desiredOutput) {
  const mu = 0.01;        // ステップサイズを大きくすると、速く収束するが、大きなギザギザが残ってしまう。  
  const output = predict(input);
  const error = desiredOutput - output;

  for (let i = 0; i < numWeights; i++) {
    weights[i] += mu * error * (input[i]||0);
  }
}

// NLMSの更新関数
function updateNLMS(input, desiredOutput) {
  const mu=0.1; // ステップサイズ
  const epsilon = 1e-6;   // 正則化パラメータ
  const output = predict(input);
  const error = desiredOutput - output;

  // inputNormは、入力値の2乗の期待値にサンプル数をかけたものなので、単なる合計になる。
  const inputNorm = input.reduce((sum, value) => sum + value * value, 0);
  const normalizedStepSize = mu / (inputNorm + epsilon);

  for (let i = 0; i < numWeights; i++) {
    weights[i] += normalizedStepSize * error * (input[i]||0);
  }
}


// LMS/NLMSアルゴリズムの適用
function apply(inputSignal, desiredOutput, updateFunc) {
  const outputSignal = [];
  for (let i = 0; i < inputSignal.length; i++) {
    const input = inputSignal.slice(Math.max(0, i - numWeights + 1), i + 1);
    const output = predict(input);
    outputSignal.push(output);
    updateFunc(input, desiredOutput[i]);
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
function demonstrateLMS(to_normalize) {
  weights = new Array(numWeights).fill(0);  // フィルタ係数の初期化
  const numSamples = 1000;
  const sineWave = generateSineWave(numSamples, 20, 0.5); // 振幅を小さくすると weightsの値も小さくなる。　周期を高くすると、weightsの値は大きくなる。
  const noise = generateNoise(numSamples, 0.5);
  const noisySignal = sineWave.map((value, index) => value + noise[index]);
  const updateFunc = (to_normalize ? updateNLMS : updateLMS);
  const filteredSignal = apply(noisySignal, sineWave, updateFunc);
  const avgPwOrig=calcAveragePower(sineWave);  
  const error=new Array(numSamples);
  for(let i=0;i<numSamples;i++) error[i]=filteredSignal[i]-sineWave[i];
  const avgPwErr=calcAveragePower(error);
  console.log('Original Signal:',sineWave);
  console.log('Noisy Signal:', noisySignal);
  console.log('Filtered Signal:', filteredSignal);
  console.log("averagePower origSignal:",avgPwOrig); // 0.12ぐらいだった(振幅0.5, 周期による変化はない)
  console.log("averagePower error:",avgPwErr); // LMS: 0.019 NLMS: 0.0074 いいね!

  const prefix=(to_normalize ? "nlms" : "lms");
  plotArrayToImage([sineWave,noisySignal],1024,512,`plots/${prefix}_demo.png`,1);
  plotArrayToImage([filteredSignal,weights],1024,512,`plots/${prefix}_demo_w.png`,1);  
  console.log("weights:",weights.join("\n"))
}

// デモンストレーションの実行

demonstrateLMS(false);
demonstrateLMS(true);
