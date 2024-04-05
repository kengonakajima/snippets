function dft(g) {
  const N=g.length;
  const G=new Array(N);
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


function idft(g) {
  const N=g.length;
  const G=new Array(N);
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

function generateSineWave(sineWave, amplitude, frequency, duration, sampleRate, numSamples) {
  const angleIncrement = 2 * Math.PI * frequency / sampleRate;
  let currentAngle = 0.0;

  for (let i = 0; i < numSamples; i++) {
    sineWave[i] = amplitude * Math.sin(currentAngle);
    currentAngle += angleIncrement;
  }
}



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

function ifft(x) {
  const n = x.length;

  // FFTの結果を複素共役にする
  const conjugate = x.map(({ re, im }) => ({ re, im: -im }));

  // 複素共役に対してFFTを適用する
  const fftResult = fft(conjugate);

  // 結果を実数部のみにして、大きさをn倍する
  const result = fftResult.map(({ re }) => re / n);

  return result;
}


//////////////////////////


const fs = require('fs');

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


/////////////////////


const N=1024;
const samples = new Float32Array(N);
const freq=100;
const sampleRate=24000;

generateSineWave(samples,1,freq,N/freq,sampleRate,N);

console.log(samples);

save_f(samples,"js_sine.raw");


// g:元の信号
const g=[];
for(let i=0;i<N;i++) g[i]={ re: samples[i], im:0 };

// まず遅いDFT
const G=dft(g); // 37ms
for(let i=0;i<N;i++){
  const hz=sampleRate/N*i;
  console.log(i,hz,G[i].re,G[i].im);
}
// IDFTして戻す
const rg=idft(G); // 37ms
for(let i=0;i<N;i++){
  const diff=rg[i].re - g[i].re;
  console.log("dft diff:",i,rg[i].re,rg[i].im, g[i].re,g[i].im,"diff:",diff);
}

// 次にFFT
const fG=fft(g); // 5ms
for(let i=0;i<N;i++) {
  const hz=sampleRate/N*i;
  console.log(i,hz,fG[i].re,fG[i].im);
}
const rfg=ifft(fG); // 5ms
for(let i=0;i<N;i++){
  const diff=rfg[i]-g[i].re;
  console.log("fft diff:",i,rfg[i],g[i].re,"diff:",diff);
}



