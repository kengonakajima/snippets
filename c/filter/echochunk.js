const fs=require("fs");
const {chunks}=require("./chunks.js");
const {
  plotArrayToImage,
  getAvg,
  getMin,
  getMax
}=require("./util.js")

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


function calcMse(errorSignal) {
  let sumSquaredError = 0;

  for (let i = 0; i < errorSignal.length; i++) {
    sumSquaredError += errorSignal[i] * errorSignal[i];
  }

  return sumSquaredError / errorSignal.length;
}


///////////


const N=512;
// 鳴らした音
let played_samples=[];
// マイクからまわりこんで来た音
let recorded_samples=[];

const delay=15;
played_samples=chunks[1];
for(let i=0;i<played_samples.length;i++) {
  recorded_samples[i]=played_samples[i-15]||0;
}
const prevchunk=chunks[0];
for(let i=0;i<delay;i++) {
  recorded_samples[i]=prevchunk[prevchunk.length-1-delay+i];
}

const FILTER_LENGTH=N;


const filter_coefficients=new Float32Array(FILTER_LENGTH); 

// FIRフィルタの処理関数
function firFilter(inputSignal, startIndex) {
  let output = 0;

  // 畳み込み演算
  for (let i = 0; i < FILTER_LENGTH; i++) {
    const signalIndex = startIndex - i;
    if (signalIndex >= 0) {
      output += filter_coefficients[i] * inputSignal[signalIndex];
    }
  }

  return output;
}

// ref,recの配列は同じ長さを前提とする。
// recには、回り込み音が含まれていることが前提。
// recに回り込み音が含まれてない場合、チャンクを伸ばす必要がある。
function echoCancel(ref,rec) {
  const estimated=new Float32Array(N);
  const err=new Float32Array(N);

  
  for(let loop=0;loop<100;loop++) {

    // エコーを推定する
    for(let i=0;i<N;i++) estimated[i]=firFilter(ref,i);


    // エコーを除去した信号をつくる。これがエラー信号となる。エラー信号が小さければ良い。
    for(let i=0;i<N;i++) err[i]=rec[i]-estimated[i];
    const eMse=calcMse(err);



    
    console.log("loop:",loop,"err: emse:",eMse);
    if(eMse<0.000001) {
      console.log("found a good coefficients, quit loop");
      break;
    }        
    // フィルタ係数を更新

    // LMS
    // いちおう山が1個突き抜けている状態にはできて、それが正しい位置を示しているが、収束が遅すぎる。
    if(false) {
      // 係数の数が増えると、ステップサイズuを小さくしないと発散する。   128だと0.2で発散するが0.1で安定する 256だと0.01 512だと0.002      
      let u=0.2;
      if(N==64) u=0.2;
      else if(N==128) u=0.1;
      else if(N==256) u=0.01;
      else if(N==512) u=0.002;

      // このuの値は、 0 < u < 2/自己相関関数の最大固有値　である必要がある。
      // 声のデータで N=512のとき、 7.934758345050212であった
      // 256のとき 3.55
      // 128のとき 0.55
      // 64のとき 0.30
      // 関係がなんかあるとは思えんのだが。
      // 
      // 音量を大きくするとuの値が同じでも発散する。
      // 音量を2倍にすると、128でも0.02にする必要があった。 512で0.0002
      // Nが大きくなってくると、収束に必要なステップが遅くなりすぎる。
      // これがLMSの限界かなあ。
      
      for(let i=0;i<N;i++) {
        for(let j=0;j<N;j++) {
          if(i-j >= 0) {
            const d=u * err[i] * ref[i - j];
            filter_coefficients[j] += d;
            //          if(j==12) console.log("hoge: i:",i,"j:",j,"f:",filter_coefficients[j],"err[i]:",err[i],"ref[i-j]",ref[i-j],"d:",d);
          }
        }
      }
    }
    // Leaky LMS
    
    // NLMS(学習同定法)
    // NLMSでは、uを動的に計算する。
    if(true) {
      const inputNorm = rec.reduce((sum, value) => sum + value * value, 0);
      let mu=0.03;
      if(N==128) mu=0.1;
      else if(N==256) mu=0.04;
      else if(N==512) mu=0.03; // 100ループかかる  たしかに発散しづらくなる!
      const epsilon=1e-6;
      const u = mu / (inputNorm + epsilon);      
      for(let i=0;i<N;i++) {
        for(let j=0;j<N;j++) {
          if(i-j >= 0) {
            const d=u * err[i] * ref[i - j];
            filter_coefficients[j] += d;
//                     if(j==12) console.log("hoge: i:",i,"j:",j,"f:",filter_coefficients[j],"err[i]:",err[i],"ref[i-j]",ref[i-j],"d:",d);
          }
        }
      }      
    }
      
//    console.log("filter_coefficients:",filter_coefficients);
    plotArrayToImage([ref,rec,estimated,err,filter_coefficients],1024,512,`plots/loop_${loop}.png`,1);
  }
  return err;
}

const rec=new Float32Array(N); // rec: マイクからの新しい入力
const ref=new Float32Array(N); // ref: 前回の再生結果(前半)


for(let i=0;i<N;i++) ref[i]=played_samples[i]*2;
for(let i=0;i<N;i++) rec[i]=recorded_samples[i]*2;

console.log("rec:",rec);
save_f(rec,"rec.raw");
console.log("ref:",ref);
save_f(ref,"ref.raw");

console.log("HOGE:",getAvg(rec),getMax(rec),getMin(rec));
const canceled=echoCancel(ref,rec);

// 係数の更新が終わっているので、この時点でのcanceledが出力である
console.log("finally output(canceled):",canceled);
save_f(canceled,"final.raw");


console.log("final Coef:",filter_coefficients)
