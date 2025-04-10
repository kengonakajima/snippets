const {
  plotArrayToImage,
  getAvg,
  getMin,
  getMax,
  calcAveragePower
}=require("./util.js")

class RLS {
  constructor(numFeatures, lambda = 0.99) {
    this.numFeatures = numFeatures; // 特徴量の数を設定
    this.lambda = lambda; // 忘却係数を設定
    this.P = Array(numFeatures).fill().map(() => Array(numFeatures).fill(1)); // 共分散行列の初期化
    this.theta = Array(numFeatures).fill(0); // パラメータベクトルの初期化
  }

  predict(x) {
    return this.theta.reduce((sum, theta, i) => sum + theta * x[i], 0); // 現在のパラメータを使用して予測値を計算
  }

  update(x, y) {
    const P = this.P; // 現在の共分散行列を取得
    const theta = this.theta; // 現在のパラメータベクトルを取得
    const lambda = this.lambda; // 忘却係数を取得
    const numFeatures = this.numFeatures; // 特徴量の数を取得

    const Px = Array(numFeatures).fill(0);
    for (let i = 0; i < numFeatures; i++) {
      for (let j = 0; j < numFeatures; j++) {
        Px[i] += P[i][j] * x[j]; // 共分散行列とデータベクトルの積を計算
      }
    }

    const g = Array(numFeatures).fill(0);
    const den = lambda + x.reduce((sum, xi, i) => sum + xi * Px[i], 0); // 分母の計算
    for (let i = 0; i < numFeatures; i++) {
      g[i] = Px[i] / den; // ゲインベクトルの計算
    }

    const newTheta = Array(numFeatures).fill(0);
    for (let i = 0; i < numFeatures; i++) {
      newTheta[i] = theta[i] + g[i] * (y - this.predict(x)); // パラメータの更新
    }
    this.theta = newTheta; // 更新されたパラメータを設定

    const newP = Array(numFeatures).fill().map(() => Array(numFeatures).fill(0));
    for (let i = 0; i < numFeatures; i++) {
      for (let j = 0; j < numFeatures; j++) {
        newP[i][j] = (P[i][j] - g[i] * Px[j]) / lambda; // 共分散行列の更新
      }
    }
    this.P = newP; // 更新された共分散行列を設定
  }
}


function firFilter(input, coefficients) {
  const output = new Array(input.length).fill(0);
  for (let i = 0; i < input.length; i++) {
    let sum = 0;
    for (let j = 0; j < coefficients.length; j++) {
      if(i-j>=0) sum += input[i - j] * coefficients[j];
    }
    output[i]=sum;
  }
  return output;
}


// デモ用のデータを生成する関数
function generateDemoData(length,freq) {
  const data = [];
  for (let i = 0; i < length; i++) {
    data.push(Math.sin(freq * Math.PI * i / 100));
  }
  return data;
}



// デモ用の参照信号を返却する関数
function getReferenceSig(n,freq) {
  return generateDemoData(n,freq);
}

// デモ用の入力信号を返却する関数
function getInputSig(n,freq) {
  const referenceSig = getReferenceSig(n,freq);
  const echoSig = firFilter(referenceSig, [0.5, 0.3, 0.2]); // エコー成分をシミュレート
  const noiseSig = generateDemoData(n,freq).map(val => val * 0.1); // ノイズ成分を追加
  return referenceSig.map((val, idx) => val + echoSig[idx] + noiseSig[idx]);
}


// ERLEを計算する関数
function calculateErle(inputSig, echoFreeSig) {
  const inputPower = inputSig.reduce((sum, val) => sum + val ** 2, 0) / inputSig.length;
  const echoFreePower = echoFreeSig.reduce((sum, val) => sum + val ** 2, 0) / echoFreeSig.length;
  const erle = 10 * Math.log10(inputPower / echoFreePower);
  return erle;
}


// エコーを除去した信号を処理する関数
function processEchoFreeSig(echoFreeSig) {
  console.log("Echo-free signal:", echoFreeSig);
}

//////////

const numSamples=32;

const numTaps=8;
const lambda=0.99;

const rls = new RLS(numTaps, lambda); // RLSオブジェクトの作成

const referenceSig = getReferenceSig(numSamples,10);
const inputSig = new Array(numSamples);
const delay=6;
for(let i=0;i<numSamples;i++) inputSig[i]=referenceSig[i-delay]||0;
console.log("referenceSig:",referenceSig);
console.log("inputSig:",inputSig);
process.exit(0);

// メインループ
for(let l=0;l<20;l++) {
  const echoPrediction = firFilter(referenceSig, rls.theta);
  const echoFreeSig = inputSig.map((val, idx) => val - echoPrediction[idx]);
  for (let i = 0; i < inputSig.length - numTaps; i++) {
    const refSigSlice = referenceSig.slice(Math.max(0, i - numTaps + 1), i + 1).reverse();
    console.log("referenceSig:",refSigSlice,referenceSig);
    rls.update(refSigSlice, inputSig[i]);
  }
  console.log("theta:",rls.theta);
  console.log("referenceSig:",referenceSig);
  console.log("inputSig:",inputSig);
  console.log("echoPrediction:",echoPrediction);
  processEchoFreeSig(echoFreeSig);
  const erle=calculateErle(inputSig,echoFreeSig);

  const errSig=referenceSig.map((val,idx)=> echoFreeSig[idx]-val);
  const errP=calcAveragePower(errSig);
  console.log("ERLE:",erle,"errP:",errP);

 
}

