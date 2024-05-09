const fs=require("fs");
const {chunks}=require("./chunks.js");
const {
  plotArrayToImage,
  getAvg,
  getMin,
  getMax,
  save_f,
  to_s,
  calcAveragePower,
  padNumber
}=require("./util.js")

///////////

// Pin フィルタ : FIRフィルタの係数のうち1つだけが > 0　であるような、極端に単純なフィルタ。
// Pinフィルタが保持する情報は、音声の遅延が100サンプルの場合、100という整数と、
// もとの音声をどの程度消すかの倍率を浮動小数で lambda としたものだけ。
// したがって係数の配列がそもそも不要となる。
function pinFilter(delay,ratio,samples,start,len) {
  const out=new Array(len);
  for(let i=0;i<len;i++) {
    out[i] = (samples[start-delay+i]||0) * ratio;
  }
  return out;  
}

//////////

const N=512;
// 全サンプル (512個が2つ)
const all_samples=[];
for(let i=0;i<N;i++) all_samples.push(chunks[0][i]);
for(let i=0;i<N;i++) all_samples.push(chunks[1][i]);


plotArrayToImage([all_samples],1024,512,`plots/all_samples.png`,1);

/*
  1024(512*2)サンプルの一部を使う
  
  |----------------------------------------------------| 1024 all
           |-----------------| 512 played
         |-----------------| 512 recorded
         ^ recStart
         
         |-| delay
 */
const recStart=100;
const delay=105; // recordedは、このサンプル数だけ、 playedよりも時間が進んでいる(多くの)
const playStart=recStart+delay;
const gain=0.3;

const err=new Float32Array(N);

let minMse=9999;
let estimatedDelay=999;
let estimatedRatio=0;

// d: 推定される遅延の候補の値
for(let d=0;d<300;d++) {
  // r: 推定される倍率の候補の値
  for(let r=0;r<=1.0;r+=0.01) {
    // 推定信号を作る
    const estimated=pinFilter(d,r,all_samples, playStart, N);
    // エコーを除去した信号をつくる。これがエラー信号となる。エラー信号が小さければ良い。
    for(let i=0;i<N;i++) err[i]=all_samples[recStart+i]*gain-estimated[i];
    const eMse=calcAveragePower(err);
    if(eMse<minMse) {
      minMse=eMse;
      estimatedDelay=d;
      estimatedRatio=r;
      console.log("better estimation: d:",d,"r:",r,"eMse:",eMse);      
    }
  }
}
// ここで、 estimatedDelayが105、 estimatedRatioが0.3付近になれば、推測成功
console.log("conclusion:",{estimatedDelay,estimatedRatio}); 



