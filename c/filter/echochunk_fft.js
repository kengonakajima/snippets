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
  calcAveragePowerComplex,  
  padNumber,
  fft_f,
  ifft_f,
  calcPowerSpectrum,
  spectrumBar
}=require("./util.js")

///////////


const N=512;
// 全サンプル (512個が2つ)
const all_samples=[];
for(let i=0;i<N;i++) all_samples.push(chunks[0][i]);
for(let i=0;i<N;i++) all_samples.push(chunks[1][i]);
const all_samples_with_noise=[];
for(let i=0;i<all_samples.length;i++) all_samples_with_noise[i]=all_samples[i]+Math.random()*0.01;

plotArrayToImage([all_samples],1024,512,`plots/all_samples.png`,1);

/*
  1024(512*2)サンプルの一部を使う
  新しいサンプルが右。
  
  |----------------------------------------------------> 1024 all  
           |-----------------| 512 played (x)
             |-----------------| 512 recorded (y)
             ^ recStart
           ^ playStart 
           |-| delay
 */

const delay=35; // recordedは、このサンプル数だけ、 playedよりも時間が進んでいる
const playStart=100;
const recStart=playStart+delay;

const x=new Float32Array(N);
for(let i=0;i<N;i++) x[i]=all_samples[playStart+i];
const y=new Float32Array(N);
for(let i=0;i<N;i++) y[i]=all_samples[recStart+i];



/*
  1. 入力信号、参照信号
  DFTは遅いので、FFTを利用する
  
  X=FFT(x)   # Subtractor::Process() 内部で利用しているrender_bufferに周波数領域のデータXが格納されている。
  Y=FFT(y)   # Subtractor::Process()

*/


const X = fft_f(x);
console.log("X:",spectrumBar(X,64),calcAveragePowerComplex(X));
const Y = fft_f(y);
console.log("Y:",spectrumBar(Y,64),calcAveragePowerComplex(Y));

const H = new Array(N); // FIRフィルタの係数(周波数領域)
for(let i=0;i<N;i++) H[i]={re:0, im:0};


const H_error = new Float32Array(N); // FIRフィルタの係数Hとは異なる。
for(let i=0;i<N;i++) H_error[i]=10000; // AEC3から


// 逐次回帰
for(let loop=0;loop<10;loop++) {

  const st=new Date().getTime();

  /*
    2. 周波数領域でフィルタし、予測誤差(エラー)信号を計算する

    S=Filter(render_buffer) # Subtractor::Process()  Sがフィルタの結果で周波数領域。 Filter()は畳み込み定理を用いて複素数のかけ算する。
    e=PredictionError(S,y)  # Subtractor::Process()  エラー信号は、 PredictionError関数内部でIFFT(S)して時間に戻してyとの差を計算。

    E=ZeroPaddedFft(e) # Subtractor::Process() エラー信号の時間領域データeをFFTして周波数領域に変換。
  */

  const S = X.map((x, i) => {
    const re = x.re * H[i].re - x.im * H[i].im;
    const im = x.re * H[i].im + x.im * H[i].re;
    return { re, im };
  });

  console.log("S:",spectrumBar(S,64),calcAveragePowerComplex(S));



  /*

    3. フィルタ係数の更新

    mu = H_error / (0.5* H_error* X2 + n * E2).

    E2は、floatの値で、パワースペクトラムを表現している。FFTではないのでby2している。
    X2もパワースペクトラムである。　 H_error は、

    void RefinedFilterUpdateGain::Computeでは H_errorはfloatの配列。
    H_errorは何が入ってるのか?

    adaptive_fir_filterで係数が H_　になっているので、同じなのでわかりにくい。

    H_error = H_error - 0.5 * mu * X2 * H_error.

    H_errorをゼロに近づける。     

    しかしなんか補正している。
    H_error = H_error + factor * erl.         


    パワースペクトラムの計算は、 fft_data.hに以下の定義がある。シンプルに複素数を2乗している。
    std::transform(re.begin(), re.end(), im.begin(), power_spectrum.begin(),
    [](float a, float b) { return a * a + b * b; });

    
    
    　　まずゲイン(G)を求めて、次に、Subtractor::Processで AdaptiveFirFilterの Adapt() を呼ぶ。
    AdaptPartitions()の内部では、
    // Adapts the filter partitions as H(t+1)=H(t)+G(t)*conj(X(t)).

    */



  const s = ifft_f(S);
  console.log("s:",s,calcAveragePower(s));

  const e = new Float32Array(N); //時間領域の予測誤差

  for(let i=0;i<N;i++) e[i]=y[i]-s[i];

  const E = fft_f(e); // E: eの周波数領域表現

  console.log("e:",e,"error AvgPower:", calcAveragePower(e)*N); // N倍したらYのavgPowと同じ値になる。


  const Xs = calcPowerSpectrum(X); // Xs: Xのパワースペクトラム
  const Es = calcPowerSpectrum(E); // Es: Eのパワースペクトラム

  console.log("Es:",Es);

  plotArrayToImage([Xs,Es],1024,512,`plots/fft_${loop}.png`,1);
  plotArrayToImage([x,y,e],1024,512,`plots/fft_xye_${loop}.png`,1);  
  
  //     mu = H_error / (0.5* H_error* X2 + n * E2).
  // partitioningしていないので n=1
  const mu = new Float32Array(N);
  for(let i=0;i<N;i++) mu[i]=H_error[i] / (0.5 * H_error[i] * Xs[i] + 1 * Es[i]);

  console.log("mu:",mu);

  //     H_error = H_error - 0.5 * mu * X2 * H_error.

  for(let i=0;i<N;i++) H_error[i]-=(0.5 * mu[i] * Xs[i] * H_error[i]);

  console.log("H_error:",H_error); 

  // G = mu * E
  const G = new Array(N);


  for(let i=0;i<N;i++) {
    G[i]={
      re: mu[i] * E[i].re,
      im: mu[i] * E[i].im
    };
  }

  console.log("G:",G);

  // H(t+1)=H(t)+G(t)*conj(X(t)).
  //      H_p_ch.re[k] += X_p_ch.re[k] * G.re[k] + X_p_ch.im[k] * G.im[k];
  //      H_p_ch.im[k] += X_p_ch.re[k] * G.im[k] - X_p_ch.im[k] * G.re[k];

  for(let i=0;i<N;i++) {
    H[i].re += X[i].re * G[i].re + X[i].im * G[i].im;
    H[i].im += X[i].re * G[i].im - X[i].im * G[i].re;
  }

  console.log("H updated:",H);

  const et=new Date().getTime();
  console.log("dt:",(et-st));
}
