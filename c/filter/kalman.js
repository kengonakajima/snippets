const {
  plotArrayToImage,
  getAvg,
  getMin,
  getMax,
  calcAveragePower
}=require("./util.js")

// カルマンフィルタのパラメータ設定
const processNoise = 1;   // プロセスノイズの分散
const measurementNoise = 1;   // 測定ノイズの分散
const initialEstimate = 0;    // 初期推定値
const initialErrorCovariance = 1;  // 初期誤差共分散

// カルマンフィルタの初期化
let estimate = initialEstimate;
let errorCovariance = initialErrorCovariance;

// カルマンフィルタの更新
function updateKalmanFilter(measurement) {
  // 予測ステップ
  const predictedEstimate = estimate;
  const predictedErrorCovariance = errorCovariance + processNoise;

  // 更新ステップ
  const kalmanGain = predictedErrorCovariance / (predictedErrorCovariance + measurementNoise);
  estimate = predictedEstimate + kalmanGain * (measurement - predictedEstimate);
  errorCovariance = (1 - kalmanGain) * predictedErrorCovariance;

  return estimate;
}

// ノイズを含む位置データの生成
function generatePositionData(numSamples, truePositionFunc) {
  const positionData = [];
  for (let i = 0; i < numSamples; i++) {
    const noise = Math.sqrt(measurementNoise) * (Math.random() - 0.5) * 2;
    positionData.push(truePositionFunc(i) + noise);
  }
  return positionData;
}

// 移動平均フィルタ
function movingAverageFilter(data, windowSize) {
  const filteredData = [];
  for (let i = 0; i < data.length; i++) {
    const start = Math.max(0, i - windowSize + 1);
    const end = i + 1;
    const window = data.slice(start, end);
    const average = window.reduce((a, b) => a + b) / window.length;
    filteredData.push(average);
  }
  return filteredData;
}

// 真実の値が変化するような場合はカルマンのほうがだいぶ性能がいい
function truePositionFunc(i) {
  // 値が変化しない。 エラー率は、移動平均: 0.066, Kalman: 0.248
  const data=[10,10,10,10, 10,10,10,10, 10,10,10,10, 10,10,10,10 ];
  // 値が変化する。 エラー率は、 移動平均:13.49 Kalman: 3.03  
  //const data=[6,4,1,4, -2,4,-1,0,  -8,-1,5,5, 7,7,9,-1 ];  
  return data[i%data.length];
}
// デモンストレーション
function demonstrateKalmanFilter() {
  const numSamples = 100;
  const positionData = generatePositionData(numSamples, truePositionFunc);

  const estimates = [];
  for (let i = 0; i < numSamples; i++) {
    const measurement = positionData[i];
    const estimate = updateKalmanFilter(measurement);
    estimates.push(estimate);
  }

  const truePositions=new Array(numSamples);
  for(let i=0;i<numSamples;i++) truePositions[i]=truePositionFunc(i);
  console.log('True Position:', truePositions);
  console.log('Noisy Measurements:', positionData);
  console.log('Kalman Filter Estimates:', estimates);

  const movingAverageEstimates = movingAverageFilter(positionData, 5);
  console.log('Moving Average Filter Estimates:', movingAverageEstimates);

  const kalmanError = estimates.map((estimate, i) => Math.pow(estimate - truePositionFunc(i), 2));
  const kalmanMSE = kalmanError.reduce((a, b) => a + b) / kalmanError.length;
  
  const movingAverageError = movingAverageEstimates.map((estimate, i) => Math.pow(estimate - truePositionFunc(i), 2));
  const movingAverageMSE = movingAverageError.reduce((a, b) => a + b) / movingAverageError.length;
 
  console.log('Kalman Filter MSE:', kalmanMSE);
  console.log('Moving Average Filter MSE:', movingAverageMSE);

  
  plotArrayToImage([positionData,estimates,movingAverageEstimates,truePositions],1024,512,`plots/kalman_demo.png`,0.05);
  
}




// デモンストレーションの実行
demonstrateKalmanFilter();
