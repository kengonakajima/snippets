const fs=require('fs');

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


// スペクトルの文字列バー表示を生成する.実数版
function spectrumBar(G) {
  const n=64;
  const step=G.length/n;
  let s="";
  for(let i=0;i<n;i++) {
    const start=Math.floor(i*step);
    let v=0;
    for(let j=start;j<start+step;j++) {
      const e=G[j]**2;
      v+=e;
    }
    if(v>2) s+="*"; else if(v>0.4)s+="+"; else if(v>0.2) s+="."; else s+=" ";
  }
  return s;
}


////////////

function dct(signal) {
  const N = signal.length;
  const output = new Array(N).fill(0);

  for (let k = 0; k < N; k++) {
    let sum = 0;
    for (let n = 0; n < N; n++) {
      sum += signal[n] * Math.cos(Math.PI * (2 * n + 1) * k / (2 * N));
    }
    output[k] = sum * (k === 0 ? Math.sqrt(1 / N) : Math.sqrt(2 / N));
  }

  return output;
}

function idct(coefficients) {
  const N = coefficients.length;
  const output = new Array(N).fill(0);

  for (let n = 0; n < N; n++) {
    let sum = 0;
    for (let k = 0; k < N; k++) {
      const factor = k === 0 ? Math.sqrt(1 / N) : Math.sqrt(2 / N);
      sum += factor * coefficients[k] * Math.cos(Math.PI * (2 * n + 1) * k / (2 * N));
    }
    output[n] = sum;
  }

  return output;
}

/////////////
function quantizationStep(index) {
  return Math.pow(1.1, index);
}

function quantize(G) {
  const quantizedCoefficients = [];
  for (let i = 0; i < G.length; i++) {
    const quantizedValue = Math.round(G[i]*100 / quantizationStep(i));
    if(quantizedValue<-128 || quantizedValue>127) throw "error";
    quantizedCoefficients.push(quantizedValue);
  }
  return quantizedCoefficients;
}

function dequantize(quantizedCoefficients) {
  const dequantizedCoefficients = [];

  for (let i = 0; i < quantizedCoefficients.length; i++) {
    const dequantizedValue = quantizedCoefficients[i] * quantizationStep(i) / 100;
    dequantizedCoefficients.push(dequantizedValue);
  }
  return dequantizedCoefficients;
}




function compressRunLength(input) {
  const output = [];
  let currentValue = input[0];
  let currentLength = 1;

  for (let i = 1; i < input.length; i++) {
    if (input[i] === currentValue && currentLength < 255) {
      currentLength++;
    } else {
      output.push(currentValue);
      output.push(currentLength);
      currentValue = input[i];
      currentLength = 1;
    }
  }

  output.push(currentValue);
  output.push(currentLength);

  return new Uint8Array(output);
}

function decompressRunLength(input) {
  const output = [];

  for (let i = 0; i < input.length; i += 2) {
    const value = input[i];
    const length = input[i + 1];

    for (let j = 0; j < length; j++) {
      output.push(value);
    }
  }

  return new Int8Array(output);
}

function compareArray(a,b) {
  if(a.length!=b.length) throw "invalid_arg";
  let diff=0;
  for(let i=0;i<a.length;i++) if(a[i]!=b[i])diff++;
  return diff;
}

///////////////////////////////////


const chunkSize = 512;
const fileData = fs.readFileSync(process.argv[2]);
const numSamples = fileData.length / 2; 
const numChunks = Math.ceil(numSamples / chunkSize);

const finalSamples=new Float32Array(numSamples);

for (let i = 0; i < numChunks; i++) {
  const start = i * chunkSize * 2;
  const end = Math.min(start + chunkSize * 2, fileData.length);
  const chunkData = fileData.slice(start, end);
  const audioData = new Int16Array(chunkData.buffer, chunkData.byteOffset, chunkData.length / 2);
  const samples=s_to_f_array(audioData);
  const G=dct(samples);
  const bar=spectrumBar(G);
  const q=quantize(G);
//  if(q[0]>5)console.log(q); // 0が多い
  const b=new Int8Array(chunkSize);
  for(let i=0;i<chunkSize;i++) b[i]=q[i];
  const rl=compressRunLength(b);
  const drl=decompressRunLength(rl);
  const diff=compareArray(b,drl);
  if(diff>0) {
    console.log("drl:",drl,"b:",b);
    throw "invalid_compress";
  }
  const dG=dequantize(drl);
  //  for(let i=0;i<chunkSize;i++) console.log("D:",dG[i].toFixed(4),G[i].toFixed(4),(dG[i]-G[i]).toFixed(4),"q:",q[i],"drl:",drl[i]);
  const dbar=spectrumBar(dG);
  console.log("i:",i,bar,"|",dbar,q[0],G[0]);
  const dSamples=idct(dG);
  for(let k=0;k<chunkSize;k++) {
    finalSamples[i*chunkSize+k]=dSamples[k]||0;
  }
}

save_f(finalSamples,"decoded.pcm");



