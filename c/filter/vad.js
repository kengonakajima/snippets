const fs = require('fs');

// floatサンプルデータをLPCM16でファイルに保存する
function save_f(buf, path) {
  const n = buf.length;
  const sb = new Int16Array(n);

  for (let i = 0; i < n; i++) {
    sb[i] = to_s(buf[i]);
  }

  fs.writeFileSync(path, Buffer.from(sb.buffer));
}

function to_s(f) {
  return Math.round(f * 32767);
}


// DFTを実行する。gは複素数の配列で、実部に音声データを入れる
function dft(g) {
  const N=g.length;
  const G=[];
  const j2pn = {
    re: 0,
    im: -2 * Math.PI / N
  };

  for (let k = 0; k < N; k++) {
    G[k] = {
      re: 0,
      im: 0
    };

    for (let n = 0; n < N; n++) {
      const exponent = {
        re: 0,
        im: j2pn.im * n * k
      };
      const expResult = exp(exponent);
      const product = multiply(g[n], expResult);
      G[k] = add(G[k], product);
    }
  }
  return G;
}

// 逆DFTを実行する。出力された配列の実部に音声データが出力される
function idft(g) {
  const N=g.length;
  const G=[];
  const j2pn = {
    re: 0,
    im: 2 * Math.PI / N
  };

  for (let k = 0; k < N; k++) {
    G[k] = {
      re: 0,
      im: 0
    };

    for (let n = 0; n < N; n++) {
      const exponent = {
        re: 0,
        im: j2pn.im * n * k
      };
      const expResult = exp(exponent);
      const product = multiply(g[n], expResult);
      G[k] = add(G[k], product);
    }
  }

  for (let k = 0; k < N; k++) {
    G[k] = {
      re: G[k].re / N,
      im: G[k].im / N
    };
  }
  return G;
}


// 複素数の足し算
function add(a, b) {
  return {
    re: a.re + b.re,
    im: a.im + b.im
  };
}

// 複素数の掛け算
function multiply(a, b) {
  return {
    re: a.re * b.re - a.im * b.im,
    im: a.re * b.im + a.im * b.re
  };
}

// 複素数の指数関数
function exp(c) {
  const expReal = Math.exp(c.re);
  return {
    re: expReal * Math.cos(c.im),
    im: expReal * Math.sin(c.im)
  };
}

// 実部の最大値を求める
function max_re(ary) {
  let v=-99999;
  for(let i=0;i<ary.length;i++) if(ary[i].re>v)v=ary[i].re;
  return v;
  
}
// 配列の最大値を求める
function max(ary) {
  let v=-99999;
  for(let i=0;i<ary.length;i++) if(ary[i]>v)v=ary[i];
  return v;
}
// スペクトルの文字列バー表示を生成する
function spectrumBar(G) {
  const n=64;
  const step=G.length/n;
  let s="";
  for(let i=0;i<n;i++) {
    const start=Math.floor(i*step);
    let v=0;
    for(let j=start;j<start+step;j++) {
      const e=energy(G[j]);
      if(e>v) v=e;
    }
    if(v>2) s+="*"; else if(v>0.4)s+="+"; else if(v>0.2) s+="."; else s+=" ";
  }
  return s;
}
function to_s(f) {
  return Math.round(f * 32767);
}
function to_f(s) {
  return s/32767.0;
}

function s_to_f_array(s_ary) {
  const out=new Float32Array(s_ary.length);
  for(let i=0;i<s_ary.length;i++) {
    out[i]=to_f(s_ary[i]);
  }
  return out;
}

// 複素数のエネルギー(振幅の2乗)を計算する
function energy(complex) {
  return complex.re*complex.re + complex.im*complex.im;
} 

// VADを実行する
function vad(G, threshold) {
  const voiceFrequencyMin = 300; // 人間の声の最小周波数（Hz）
  const voiceFrequencyMax = 3000; // 人間の声の最大周波数（Hz）
  const sampleRate = 44100; // サンプリングレート（Hz）
  const fftSize = G.length;

  // 人間の声の周波数帯域に対応する周波数ビンのインデックスを計算
  const kMin = Math.floor((voiceFrequencyMin * fftSize) / sampleRate);
  const kMax = Math.floor((voiceFrequencyMax * fftSize) / sampleRate);

  // 人間の声の周波数帯域のエネルギーを合計
  let voiceEnergySum = 0;
  for (let k = kMin; k <= kMax; k++) {
    voiceEnergySum += energy(G[k]);
  }
  return (voiceEnergySum > threshold) ;
}
function avgEnergy(G) {
  let sum=0;
  for (let i = 0;i<G.length;i++) {
    sum += energy(G[i]);
  }
  return sum/G.length;
}


///////////////////

const chunkSize = 512;
const fileData = fs.readFileSync(process.argv[2]);
const numSamples = fileData.length / 2; 
const numChunks = Math.ceil(numSamples / chunkSize);
const finalSamples=new Float32Array(numSamples);

// ノイズ削減用のフィルタ
const wienerFilter = new Array(chunkSize);
for(let i=0;i<chunkSize;i++) wienerFilter[i]={re:0,im:0};
const noisePSD = new Array(chunkSize).fill(0);

for (let i = 0; i < numChunks; i++) {
  const start = i * chunkSize * 2;
  const end = Math.min(start + chunkSize * 2, fileData.length);
  const chunkData = fileData.slice(start, end);
  const audioData = new Int16Array(chunkData.buffer, chunkData.byteOffset, chunkData.length / 2);
  const samples=s_to_f_array(audioData);
//  console.log(start,end,"audioData:",audioData[0],max(audioData));

  // DFTの結果をVADやNSで利用
  const g=[];
  for(let k=0;k<chunkSize;k++) g[k]={ re: samples[k], im:0 };
  const G=dft(g);
  const is_voice=vad(G,4000); // 2000にするとvoice_canでたまにtrueになる
  const bar=spectrumBar(G);
  const e=avgEnergy(G);

  // 声がないところがノイズと仮定
  if(!is_voice) {
    const alpha=0.9;
    for(let k=0;k<chunkSize;k++) {
      const spectrumPSD=energy(G[k]);
      noisePSD[k] = alpha * noisePSD[k] + (1 - alpha) * spectrumPSD; 
    }
  }
  
  // ウィーナーフィルタの計算
  const newWienerFilter = new Array(chunkSize);
  for(let k=0;k<chunkSize;k++) {
    const spectrumPSD = energy(G[k]);
    const signalPSD = spectrumPSD - noisePSD[k];
    const gain = Math.max(signalPSD / spectrumPSD, 0);
    newWienerFilter[k] = {re:gain, im:0 };
  }
  // ウィーナーフィルタの更新
  const smoothingFactor = 0.5;
  for (let k = 0; k < chunkSize; k++) {
    const prevFilterReal = wienerFilter[k].re;
    const prevFilterImag = wienerFilter[k].im;
    const newFilterReal = newWienerFilter[k].re;
    const newFilterImag = newWienerFilter[k].im;
    const smoothedFilterReal = smoothingFactor * prevFilterReal + (1 - smoothingFactor) * newFilterReal;
    const smoothedFilterImag = smoothingFactor * prevFilterImag + (1 - smoothingFactor) * newFilterImag;
    wienerFilter[k] = {
      re: smoothedFilterReal,
      im: smoothedFilterImag
    };
  }
  

  // ノイズ除去の適用
  const filteredSpectrum = new Array(chunkSize);
  for (let k = 0; k < chunkSize; k++) {
    filteredSpectrum[k] = multiply(G[k], wienerFilter[k]);
  }


  const fE=avgEnergy(filteredSpectrum);

  // 時間領域への逆変換
  const fG = idft(filteredSpectrum);
  for(let k=0;k<chunkSize;k++) finalSamples[i*chunkSize+k]=fG[k].re;
  console.log(bar,"max:",max(audioData),max_re(G),"is_voice:",is_voice,"e:",e,"fE:",fE);
}

save_f(finalSamples,"filtered.pcm");
