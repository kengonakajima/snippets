function dft(g, G, N) {
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
}


function idft(g, G, N) {
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

const g=[];
for(let i=0;i<N;i++) g[i]={ re: samples[i], im:0 };

const G=[];
dft(g,G,N);
console.log("G:",G);
for(let i=0;i<N;i++){
  const hz=sampleRate/N*i;
  console.log(i,hz,G[i].re,G[i].im);
}

const rg=[];
idft(G,rg,N);
for(let i=0;i<N;i++){
  const diff=rg[i].re - g[i].re;
  console.log(i,rg[i].re,rg[i].im, g[i].re,g[i].im,"diff:",diff);
}


