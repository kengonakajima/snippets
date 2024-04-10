const fs=require("fs");

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


const N=10;
// 鳴らした音
const played_samples=[
    0,0,1,0.5,0, 0,0,0,0,0
];
// マイクからまわりこんで来た音
const recorded_samples=[
    0,0,0,0,0, 0,0.2,0.1,0,0
];


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
  const canceled=new Float32Array(N);
  const err=new Float32Array(N);

  for(let loop=0;loop<100;loop++) {
    console.log("loop:",loop);

    // エコーを推定する
    for(let i=0;i<N;i++) estimated[i]=firFilter(ref,i);
    console.log("estimated:",estimated);

    // エコーを除去した信号をつくる。これがエラー信号となる。エラー信号が小さければ良い。
    for(let i=0;i<N;i++) err[i]=rec[i]-estimated[i];
    const mse=calcMse(err);
    console.log("err: mse:",mse,err);
    if(mse<0.000001) {
      console.log("found a good coefficients, quit loop");
      break;
    }        
    // フィルタ係数を更新
    const u=0.2;
    for(let i=0;i<N;i++) {
      for(let j=0;j<N;j++) {
        if(i-j >= 0) {
          filter_coefficients[j] += u * err[i] * ref[i - j];
        }
      }
    }
    console.log("filter_coefficients:",filter_coefficients);
    console.log("===============\n");
  }
  return err;
}

const rec=new Float32Array(N); // rec: マイクからの新しい入力
const ref=new Float32Array(N); // ref: 前回の再生結果(前半)


for(let i=0;i<N;i++) ref[i]=played_samples[i];
for(let i=0;i<N;i++) rec[i]=recorded_samples[i];

console.log("rec:",rec);
save_f(rec,"rec.raw");
console.log("ref:",ref);
save_f(ref,"ref.raw");

const canceled=echoCancel(ref,rec);

// 係数の更新が終わっているので、この時点でのcanceledが出力である
console.log("finally output(canceled):",canceled);
save_f(canceled,"final.raw");


