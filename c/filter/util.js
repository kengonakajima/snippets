const assert = require("assert");
const fs = require('fs');
const { createCanvas } = require('canvas');
const numeric=require("numeric")

// "******      " のような文字列を返す
function getVolumeBar(l16sample) {
  const vol=Math.abs(l16sample) || 0;
  const bar = vol / 1024;
  const space = 32-bar;
  return "*".repeat(bar)+" ".repeat(space); 
}


function createJitterBuffer(jitter) {
  const b={};
  b.samples=[]; // i16le
  b.jitter=jitter;
  b.needJitter=true;
  b.push=function(sample) {
    this.samples.push(sample);
    if(this.needJitter && this.samples.length>this.jitter) {
      console.log("jitterbuffer: filled jitter:",this.jitter);
      this.needJitter=false;
    }
  }
  b.shift=function() {
    return this.samples.shift();
  }
  b.clear=function() {
    this.samples=[];
  }
  b.used=function(){return this.samples.length;}
  return b;
}
function getAvg(ary){
  let t=0;
  for(let i in ary) t+=ary[i];
  return t/ary.length;
}
function getMin(ary){
  let minv=9999999;
  for(let i in ary) {
    if(ary[i]<minv) minv=ary[i];
  }
  return minv;  
}
function getMax(ary){
  let maxv=-9999999;
  for(let i in ary) {
    if(ary[i]>maxv) maxv=ary[i];
  }
  return maxv;
}

function appendBinaryToFile(fileName, array) {
  // 配列をバッファに変換
  const buffer = Buffer.from(array.buffer);

  // ファイルにバッファを追記
  fs.appendFileSync(fileName, buffer);
}

function to_f(s) {
  return s/32767.0;
}
function to_s(f) {
  return Math.round(f * 32767);
}

function to_f_array(s_ary) {
  const out=new Float32Array(s_ary.length);
  for(let i=0;i<s_ary.length;i++) {
    out[i]=to_f(s_ary[i]);
  }
  return out;
}
function to_s_array(f_ary) {
  const out=new Int16Array(f_ary.length);
  for(let i=0;i<f_ary.length;i++) {
    out[i]=to_s(f_ary[i]);
  }
  return out;  
}

function save_f(buf, path) {
  const n = buf.length;
  const sb = new Int16Array(n);

  for (let i = 0; i < n; i++) {
    sb[i] = to_s(buf[i]);
  }

  fs.writeFileSync(path, Buffer.from(sb.buffer));
}
function append_f(buf,path) {
  const n = buf.length;
  const sb = new Int16Array(n);

  for (let i = 0; i < n; i++) {
    sb[i] = to_s(buf[i]);
  }

  appendBinaryToFile(path,sb);
}

function rm(path) {
  try {
   fs.unlinkSync(path); 
  }catch(e) {
    
  }
}

function calcERLE(inputSignal, outputSignal) {
  const inputPower = calcAveragePower(inputSignal);
  const residualEchoPower = calcAveragePower(outputSignal);
  
  const erle = 10 * Math.log10(inputPower / residualEchoPower);
  return erle;
}

function calcAveragePower(signal) {
  const sum = signal.reduce((acc, sample) => acc + sample ** 2, 0);
  const averagePower = sum / signal.length;
  return averagePower;
}
function calcAveragePowerComplex(fftResult) {
  const sum = fftResult.reduce((acc, { re, im }) => acc + re ** 2 + im ** 2, 0);
  const averagePower = sum / fftResult.length;
  return averagePower;
}
function calcPowerSpectrum(complexArray) {
  const out=new Float32Array(complexArray.length);
  for(let i=0;i<complexArray.length;i++) {
    out[i]=complexArray[i].re * complexArray[i].re + complexArray[i].im * complexArray[i].im;
  }
  return out;
}
function padNumber(number, width, paddingChar = ' ') {
  return number.toString().padStart(width, paddingChar);
}
function totMag(a,b) {
  let tot=0;
  for(let i=0;i<a.length;i++) {
    const d=a[i]-b[i];
    tot+=d*d;
  }
  return tot;
}

// {re,im}
function fft(x) {
  const n = x.length;

  if (n === 1) {
    return x;
  }

  const even = [];
  const odd = [];

  for (let i = 0; i < n; i++) {
    if (i % 2 === 0) {
      even.push(x[i]);
    } else {
      odd.push(x[i]);
    }
  }

  const evenFFT = fft(even);
  const oddFFT = fft(odd);

  const result = new Array(n);

  for (let k = 0; k < n / 2; k++) {
    const angle = -2 * Math.PI * k / n;
    const twiddle = {
      re: Math.cos(angle),
      im: Math.sin(angle)
    };

    const t = {
      re: twiddle.re * oddFFT[k].re - twiddle.im * oddFFT[k].im,
      im: twiddle.re * oddFFT[k].im + twiddle.im * oddFFT[k].re
    };

    result[k] = {
      re: evenFFT[k].re + t.re,
      im: evenFFT[k].im + t.im
    };

    result[k + n / 2] = {
      re: evenFFT[k].re - t.re,
      im: evenFFT[k].im - t.im
    };
  }

  return result;
}

// {re,im}
function ifft(x) {
  const n = x.length;

  // FFTの結果を複素共役にする
  const conjugate = x.map(({ re, im }) => ({ re, im: -im }));

  // 複素共役に対してFFTを適用する
  const fftResult = fft(conjugate);

  return fftResult;
}
function ifft_f(x) {
  const o=ifft(x);
  // 結果を実数部のみにして、大きさをn倍する
  const result = o.map(({ re }) => re / x.length);
  return result;
}

// 複素数ではなくfloatの配列を受け取るFFT
function fft_f(floats) {
  const n=floats.length;
  const g=to_c_array(floats);
  const G=fft(g);
  return G;
}

// float to {re,im}
function to_c_array(floats) {
  const out=new Array(floats.length);
  for(let i=0;i<floats.length;i++) out[i]={ re: floats[i], im:0 };
  return out;
}

function spectrumBar(s,num) {
  const out=[];
  for(let i=0;i<num;i++) out[i]=' ';
  const step=s.length/num;  
  for(let i=0;i<s.length;i++) {
    const outi=parseInt(i/step);
    let e=s[i].re;
    if(e<0)e*=-1;
    if(e>out[outi])out[outi]=e;
  }
  for(let i=0;i<num;i++) {
    if(out[i]>1) out[i]='*';
    else if(out[i]>0.5) out[i]='+';
    else if(out[i]>0.2) out[i]='-';
    else if(out[i]>0.05) out[i]='.';
    else out[i]=' ';
  }
  
  return out.join("");
}

// 複素数の足し算
function addComplex(a, b) {
  return {
    re: a.re + b.re,
    im: a.im + b.im
  };
}

// 複素数の掛け算
function multiplyComplex(a, b) {
  return {
    re: a.re * b.re - a.im * b.im,
    im: a.re * b.im + a.im * b.re
  };
}

function multiplyComplexArray(a, b) {
  const result = [];
  for (let i = 0; i < a.length; i++) {
    const re = a[i].re * b[i].re - a[i].im * b[i].im;
    const im = a[i].re * b[i].im + a[i].im * b[i].re;
    result.push({re,im});
  }
  return result;
}


// 複素数の指数関数
function expComplex(c) {
  const expReal = Math.exp(c.re);
  return {
    re: expReal * Math.cos(c.im),
    im: expReal * Math.sin(c.im)
  };
}

function conjugate(complexArray) {
  for(let i=0;i<complexArray.length;i++) complexArray[i]={re: complexArray[i].re, im: complexArray[i].im*-1} ;
  return complexArray;
}


function plotArrayToImage(data_list, width, height, outputFilename,scale=1) {
  const canvas = createCanvas(width, height);
  const ctx = canvas.getContext('2d');

  for(const data of data_list) {
    for(let i=0;i<data.length;i++) data[i]=data[i]*scale;
  }

  
  // 背景を白で塗りつぶす
  ctx.fillStyle = 'white';
  ctx.fillRect(0, 0, width, height);

  // グラフの軸を描画
  ctx.strokeStyle = 'black';
  ctx.lineWidth = 1;
  ctx.beginPath();
  ctx.moveTo(50, 0);
  ctx.lineTo(50, height/2);
  ctx.lineTo(width - 20, height/2);
  ctx.stroke();

  const colors=['red','green','blue','orange','purple','black','gray'];
  // データをプロット
  const l=data_list[0].length;  
  for(let di=0;di<data_list.length;di++) {
    const data=data_list[di];
    ctx.strokeStyle = colors[di];
    ctx.lineWidth = 1;
    ctx.beginPath();
    ctx.moveTo(50, height/2 - (data[0] * (height/2)));
    for (let i = 1; i < l; i++) {
      const d=data[i]||0;
      const x = 50 + (i * (width - 70) / (l - 1));
      const y = height/2 - (d * (height/2));
      if(y>=0 && y<=height && x>=0 && x<=width) ctx.lineTo(x, y);
    }
    ctx.stroke();
  }

  
  // 画像をファイルに保存
  const buffer = canvas.toBuffer('image/png');
  fs.writeFileSync(outputFilename, buffer);
}


function loadLPCMFileSync(path,chunkSize) {
  const fileData = fs.readFileSync(path);
  if(!chunkSize) {
    return new Int16Array(fileData.buffer);
  } else { 
    const numSamples = fileData.length / 2; 
    const numChunks = Math.ceil(numSamples / chunkSize);
    const out=[];
    for (let i = 0; i < numChunks; i++) {
      const start = i * chunkSize * 2;
      const end = Math.min(start + chunkSize * 2, fileData.length);
      const chunkData = fileData.slice(start, end);
      const audioData = new Int16Array(chunkData.buffer, chunkData.byteOffset, chunkData.length / 2);
      out.push(audioData);
    }
    return out;
  }
}


// FIRフィルタの処理関数
function firFilterSingle(inputSignal,startIndex,coefs) {
  let output = 0;
  // 畳み込み演算
  for (let i = 0; i < coefs.length; i++) {
    const signalIndex = startIndex - i;
    if (signalIndex >= 0) {
      output += coefs[i] * inputSignal[signalIndex];
    }
  }
  return output;
}
function firFilter(inputSignal,coefs,N) {
  if(inputSignal.length<N || coefs.length!=N){
    console.log("invalid len:",inputSignal.length,coefs.length);
    throw "invalid_arg";
  }
  const estimated=new Float32Array(N);
  const n=coefs.length;
  for(let i=0;i<N;i++) estimated[i]=firFilterSingle(inputSignal,i,coefs);
  return estimated;
}

function firFilterFFT(inputSignal, filterCoefficients) {
  if(inputSignal.length!=filterCoefficients.length) throw "inputSignal_not_FL";
  //const L = filterCoefficients.length;

  const fl=filterCoefficients.length;
  let filteredSignal = new Array(inputSignal.length + fl - 1).fill(0);

  const filterFFT = fft_f(filterCoefficients);

  const segmentStart = 0;
  const segmentEnd = Math.min(segmentStart + fl, inputSignal.length);
  const segment = inputSignal.slice(segmentStart, segmentEnd);
  const paddedSegment = [...segment, ...new Array(fl - segment.length).fill(0)];

  const segmentFFT = fft_f(paddedSegment);
  const filteredSegmentFFT = multiplyComplexArray(segmentFFT, filterFFT);
  const filteredSegment = ifft_f(filteredSegmentFFT);

  for (let j = 0; j < fl; j++) {
    filteredSignal[segmentStart + j] += filteredSegment[j];
  }

  return {filtered: filteredSignal.slice(0, inputSignal.length) };
}
function findMax(array) {
  let max=-999999;
  let out_ind=-1;
  for(let i=0;i<array.length;i++) {
    if(array[i]>max) {
      out_ind=i;
      max=array[i];
    }
  }
  return {index: out_ind, value: max};
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



function transposeMatrix(matrix) {
  const rows = matrix.length;
  const cols = matrix[0].length;
  
  const out = [];
  
  for (let j = 0; j < cols; j++) {
    out[j] = [];
    for (let i = 0; i < rows; i++) {
      out[j][i] = matrix[i][j];
    }
  }
  
  return out;
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

function addMatrices(matrix1, matrix2) {
  // 行列のサイズが等しいかチェック
  if (matrix1.length !== matrix2.length || matrix1[0].length !== matrix2[0].length) {
    throw new Error("行列のサイズが異なります");
  }

  const result = [];
  for (let i = 0; i < matrix1.length; i++) {
    result[i] = [];
    for (let j = 0; j < matrix1[0].length; j++) {
      result[i][j] = matrix1[i][j] + matrix2[i][j];
    }
  }
  return result;
}

function subtractMatrices(matrix1, matrix2) {
  // 行列のサイズが等しいかチェック
  if (matrix1.length !== matrix2.length || matrix1[0].length !== matrix2[0].length) {
    throw new Error("行列のサイズが異なります");
  }

  const result = [];
  for (let i = 0; i < matrix1.length; i++) {
    result[i] = [];
    for (let j = 0; j < matrix1[0].length; j++) {
      result[i][j] = matrix1[i][j] - matrix2[i][j];
    }
  }
  return result;
}

// dxdの単位行列を作る
function createMatrix(d) {
  const o=[];
  for(let i=0;i<d;i++) {
    o[i]=[];
    for(let j=0;j<d;j++) {
      let v=0;
      if(j==i)v=1;
      o[i][j]=v;
    }
  }
  return o;
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

function divideMatrixByScalar(matrix, scalar) {
  if (scalar === 0) {
    throw new Error("スカラー値が0です。ゼロ除算は許可されていません。");
  }

  const result = [];
  for (let i = 0; i < matrix.length; i++) {
    result[i] = [];
    for (let j = 0; j < matrix[0].length; j++) {
      result[i][j] = matrix[i][j] / scalar;
    }
  }
  return result;
}

function multiplyMatrixByScalar(matrix, scalar) {
  const result = [];
  for (let i = 0; i < matrix.length; i++) {
    result[i] = [];
    for (let j = 0; j < matrix[0].length; j++) {
      result[i][j] = matrix[i][j] * scalar;
    }
  }
  return result;
}



exports.calculateCorrelationMatrix=calculateCorrelationMatrix;
exports.calculateMaxEigenvalue=calculateMaxEigenvalue;
exports.getMax=getMax;
exports.getMin=getMin;
exports.getAvg=getAvg;
exports.createJitterBuffer=createJitterBuffer;
exports.getVolumeBar = getVolumeBar;
exports.appendBinaryToFile = appendBinaryToFile;
exports.to_f=to_f;
exports.to_s=to_s;
exports.to_f_array=to_f_array;
exports.to_s_array=to_s_array;
exports.save_f = save_f;
exports.append_f = append_f;
exports.rm=rm;
exports.calcERLE = calcERLE;
exports.calcAveragePower = calcAveragePower;
exports.calcAveragePowerComplex = calcAveragePowerComplex;
exports.padNumber=padNumber;
exports.totMag=totMag;
exports.fft=fft;
exports.ifft=ifft;
exports.ifft_f=ifft_f;
exports.to_c_array=to_c_array;
exports.fft_f=fft_f;
exports.spectrumBar=spectrumBar;
exports.multiplyComplex=multiplyComplex;
exports.multiplyComplexArray=multiplyComplexArray;
exports.addComplex=addComplex;
exports.expComplex=expComplex;
exports.conjugate=conjugate;
exports.plotArrayToImage=plotArrayToImage;
exports.loadLPCMFileSync=loadLPCMFileSync;
exports.firFilter=firFilter;
exports.firFilterFFT=firFilterFFT;
exports.findMax=findMax;
exports.transposeMatrix=transposeMatrix;
exports.multiplyMatrices=multiplyMatrices;
exports.invertMatrix=invertMatrix;
exports.addMatrices=addMatrices;
exports.subtractMatrices=subtractMatrices;
exports.createMatrix=createMatrix;
exports.dotProduct=dotProduct;
exports.divideMatrixByScalar=divideMatrixByScalar;
exports.multiplyMatrixByScalar=multiplyMatrixByScalar;
exports.calcPowerSpectrum=calcPowerSpectrum;
