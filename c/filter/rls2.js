class RLS {
  constructor(numFeatures, lambda = 0.99) {
    this.numFeatures = numFeatures; // 特徴量の数を設定
    this.lambda = lambda; // 忘却係数を設定
    this.P = Array(numFeatures).fill().map(() => Array(numFeatures).fill(1)); // 共分散行列の初期化
    this.theta = Array(numFeatures).fill(0); // パラメータベクトルの初期化
  }

  predict(x) {
    return this.theta.reduce((sum, th, i) => {
      const newsum=sum + th * x[i];
      console.log("predict: th:",th,"i:",i,"x[i]:",x[i],"sum:",sum,"newsum:",newsum);
      return newsum;
    }, 0); // 現在のパラメータを使用して予測値を計算
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

// 使用例
const rls = new RLS(2); // RLSオブジェクトの作成（特徴量の数は2）

// データセット ( 0番目+1番目*2)
const data_ = [
  { x: [1, 1], y: 3 },
  { x: [1, 2], y: 5 },
  { x: [2, 1], y: 3 },
  { x: [2, 2], y: 6 },
  { x: [0, 2], y: 4 },
  { x: [2, 0], y: 2 },
  { x: [4, 4], y: 12 },
  { x: [10, 0], y: 10 },
  { x: [100, 0], y: 100 },
  { x: [0, 100], y: 200 },            
];
// データセット:つねに1番目と同じ値になる
const data = [
  { x: [0, 1], y: 1 },
  { x: [0, 2], y: 2 },
  { x: [0, 10], y: 10 },
  { x: [0, 20], y: 20 },
  { x: [1, 1], y: 1 },
  { x: [2, 2], y: 2 },
  { x: [5, 10], y: 10 },
  { x: [10, 20], y: 20 },
  { x: [1, 0], y: 0 },
  { x: [2, 0], y: 0 },
  { x: [8, 0], y: 0 },
  { x: [9, 0], y: 0 },
];

// 学習
data.forEach(({ x, y }) => {
  console.log("Predicting..");
  const prediction = rls.predict(x); // 現在のパラメータを使用して予測値を計算
  console.log(`Input: [${x}], Prediction: ${prediction}, Actual: ${y}`); // 入力、予測値、実際の値を表示
  console.log("Updating..");  
  rls.update(x, y); // パラメータの更新
  console.log("theta:",rls.theta);
});

// テスト
const testInput = [
  [3, 3],
  [3, 0],
  [0, 3],    
];
for(const t of testInput) {
  const testPrediction = rls.predict(t); // テストデータポイントの予測値を計算
  console.log(`Test Input: [${t}], Test Prediction: ${testPrediction}`); // テストデータポイントと予測値を表示  
}


///////////////////////////

// sin wave
const referenceSig= [
              0,   0.309016,
       0.587785,   0.809016,
       0.951056,          1,
       0.951056,   0.809015,
       0.587785,   0.309016,
              0,  -0.309019,
      -0.587785,  -0.809016,
      -0.951056,         -1,
      -0.951056,  -0.809016,
      -0.587785,  -0.309016,
              0,   0.309016,
       0.587785,   0.809016,
       0.951056,          1,
       0.951056,   0.809016,
       0.587785,   0.309016,
              0, -0.3090164
];
// 6個ずれてる
const inputSig= [
              0,         0,
              0,         0,
              0,         0,
              0,  0.309016,
       0.587785,  0.809016,
       0.951056,         1,
       0.951056,  0.809016,
       0.587785,  0.309016,
              0, -0.309016,
      -0.587785, -0.809016,
      -0.951056,        -1,
      -0.951056, -0.809016,
      -0.587785, -0.309016,
              0,  0.309016,
       0.587785,  0.809016,
       0.951056,         1
];
const data2=[
  { x: []}  
];
