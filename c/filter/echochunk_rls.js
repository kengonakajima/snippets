// LMSではなくRLSを用いる
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
  firFilter,
  padNumber,
  createMatrix,
  transposeMatrix,
  multiplyMatrices,
  invertMatrix,
  divideMatrixByScalar,
  subtractMatrices,
  multiplyMatrixByScalar,
  addMatrices,  
}=require("./util.js")

///////////


// rls0.js (清水版)の重回帰バージョン
// https://yuyumoyuyu.com/2020/12/27/recursiveleastsquares/
class RLSShimizu {
  constructor(d,lambda) {
    this.d=d;
    this.theta=[];
    for(let i=0;i<d;i++) this.theta.push([0]); // rls1.jsではd次の行ベクトルだったが、清水版ではthetaは列ベクトルである
    this.P = createMatrix(d);
    this.lambda=lambda;
  }
  predict(x) {
    console.log("predict: x:",x,"theta:",this.theta);
    let sum=0;
    for(let i=0;i<this.theta.length;i++) {
      sum+=x[i]*this.theta[i];
    }
    return sum;
  }
  /*
    P[n] = P[n-1] / ( lambda + x(n) * P[n-1] * x(n)^T )
                     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                       分母 s : scalar

    gain = ( P[n-1] * x(n)^T ) / ( lambda + x(n) * P[n-1] * x(n)^T )
            ~~~~~~~~~~~~~~~~     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
              分子 g0                分母 s
              
    theta[n] = theta[n-1] + gain * ( y(n) - x(n) * theta[n-1] )
                                            ~~~~~~~~~~~~~~~~~~~
                                              t0:  1xd * dx1 -> scalar
                                    ~~~~~~~~~~~~~~~~~~~~~~~~~~
                                        t1 : scalar
                            ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                               t2 : 列ベクトル
   */
  
  update(x,y) {
    const x_n=[x];
    const x_n_t=transposeMatrix(x_n);
    const s0=multiplyMatrices(x_n,this.P);
    const s1=multiplyMatrices(s0,x_n_t);
    const s=this.lambda + s1[0][0]; // scalar
    const P_n=multiplyMatrixByScalar(this.P, 1/s); // dxd行列
    const g0=multiplyMatrices(this.P, x_n_t); // 列ベクトル
    const gain=multiplyMatrixByScalar(g0, 1/s); // 列ベクトル
    const t0 = multiplyMatrices(x_n,this.theta); // scalar
    const t1 = y-t0[0][0];
    const t2 = multiplyMatrixByScalar(gain,t1);
    const theta_n = addMatrices(this.theta,t2);
    console.log({x_n,x_n_t,s0,s1,s,P_n,g0,gain,t0,theta: this.theta, t1, t2, theta_n});
    this.theta=theta_n;
    this.P=P_n;
  }

  init(data) {
    // w = (X^T * X)^-1 * X^T * y
    // Xは、xが全部並んでいるデザイン行列. 各行が各データを表しており、各列が各入力変数を表す。
    // デザイン行列Xを作る
    const N=data.length; // データの列の長さ
    const M=data[0].x.length; // データの行の長さ
    const X=[]; // 行列は2次元配列(ベクトルの配列)
    const Y=[]; // 1次元配列、スカラ値の配列
    for(let i=0;i<N;i++) {
      X.push(data[i].x);
      Y.push([data[i].y]);  
    }
  
    
    // X^Tを作成する
    const X_T=transposeMatrix(X);
    const P=multiplyMatrices(X_T,X); // P = X^T * X
    const invP=invertMatrix(P);
    const Q=multiplyMatrices(invP,X_T); // Q = P^-1 * X^T
    const w=multiplyMatrices(Q,Y); // これが偏回帰係数  w: [ [ 0 ], [ 1.9999999999999998 ] こういうの
    console.log({N,M,X,Y,X_T,P,invP,Q,w}); 
    this.theta=w;
    this.P=P;
  }
  

}


/*
  init()をどのようにすればよいか?

  学習する元となるデータがそもそもない。
  initの入力は 答え(y)を含むデータだが、それがそもそも存在しない。
  なのでまずぜんぶ0という値を用意してみる。
  
 */


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

const initData = [
  { x: recorded_samples, y: 0},
];

const rls=new RLSShimizu(N,0.5);
rls.init(initData);

console.log("rls:",rls);

process.exit(0);

// ref,recの配列は同じ長さを前提とする。
// recには、回り込み音が含まれていることが前提。
// recに回り込み音が含まれてない場合、チャンクを伸ばす必要がある。
function echoCancel(ref,rec) {
  const err=new Float32Array(N);

  
  for(let loop=0;loop<100;loop++) {
    // エコーを推定する    
    const estimated=firFilter(ref,filter_coefficients,N);

    // エコーを除去した信号をつくる。これがエラー信号となる。エラー信号が小さければ良い。
    for(let i=0;i<N;i++) err[i]=rec[i]-estimated[i];
    const eMse=calcAveragePower(err);



    
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
    const pad=padNumber(loop,3,'0');
    plotArrayToImage([ref,rec,estimated,err,filter_coefficients],1024,512,`plots/loop_${pad}.png`,1);
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
