const recorder = require('node-record-lpcm16'); // nodeモジュールを読み込む
const fs=require("fs");

const FREQ=48000;
const AEC3_SAMPLES_PER_FRAME=FREQ/100;

const aec3 = require('./aec3.js');

let aec3Wrapper={ initialized: false};
aec3.onRuntimeInitialized = () => {
  aec3Wrapper.init=aec3.cwrap("aec3_init","void",["number","number"]);
  aec3Wrapper.debug_print=aec3.cwrap("aec3_debug_print","void",[]);
  aec3Wrapper.get_metrics_echo_return_loss_enhancement=aec3.cwrap("aec3_get_metrics_echo_return_loss_enhancement","number",[]);
  aec3Wrapper.get_metrics_delay_ms=aec3.cwrap("aec3_get_metrics_delay_ms","number",[]);
  aec3Wrapper.update_ref_frame=aec3.cwrap("aec3_update_ref_frame","void",["number","number"]);  
  aec3Wrapper.update_ref_frame_wrapped = function(i16ary) {
    const num=i16ary.length;
    const ptr=aec3._malloc(Int16Array.BYTES_PER_ELEMENT*num);
    if(ptr==null) throw new "_malloc fail";
    aec3.HEAP16.set(i16ary, ptr/Int16Array.BYTES_PER_ELEMENT);
    this.update_ref_frame(ptr,num);
  }
  aec3Wrapper.update_rec_frame=aec3.cwrap("aec3_update_rec_frame","void",["number","number"]);  
  aec3Wrapper.update_rec_frame_wrapped = function(i16ary) {
    const num=i16ary.length;
    const ptr=aec3._malloc(2*num);
    if(ptr==null) throw new "_malloc fail";
    aec3.HEAP16.set(i16ary, ptr/2);
    this.update_rec_frame(ptr,num);
  }
  aec3Wrapper.process=aec3.cwrap("aec3_process","void",["number","number","number","number"]);  
  aec3Wrapper.process_wrapped = function(ms,i16ary,ns) {
    const num=i16ary.length;
    const ptr=aec3._malloc(2*num);
    if(ptr==null) throw new "_malloc fail";
    aec3.HEAP16.set(i16ary, ptr/2);
    this.process(ms,ptr,num,ns);
    const data=aec3.HEAP16.subarray(ptr/2,ptr/2+num);
    for(let i=0;i<num;i++)i16ary[i]=data[i];
  }
  
  aec3Wrapper.debug_print();
  aec3Wrapper.init(4,0);
  aec3Wrapper.initialized=true;  
}


// "******      " のような文字列を返す
function getVolumeBar(l16sample) {
  const vol=Math.abs(l16sample);
  const bar = vol / 1024;
  const space = 32-bar;
  return "*".repeat(bar)+" ".repeat(space); 
}

///////////
// recording
const g_samples=[]; // lpcm16
let g_rec_max_sample=0, g_play_max_sample=0;
let g_enh=0;

recorder
  .record({
    sampleRate: FREQ, // マイクデバイスのサンプリングレートを指定
    channels: 1,  // チャンネル数を指定(モノラル)              
    recordProgram: 'rec', // 録音用のバックエンドプログラム名を指定
  })
  .stream()
  .on('error', console.error) // エラーが起きたときにログを出力する
  .on('data', function(data) { // マイクからデータを受信する無名コールバック関数
    const sampleNum=data.length/2;
    g_rec_max_sample=0;
    for(let i=0;i<sampleNum;i++) {
      const sample=data.readInt16LE(i*2);
      g_samples.push(sample);
      if(sample>g_rec_max_sample)g_rec_max_sample=sample;
    }
//    console.log("rec:",g_samples.length,"[0]:",g_samples[0]);
  });

/////////////////////
// playing

const Readable=require("stream").Readable; 
const Speaker=require("speaker");

const player=new Readable();
player.ref=[];
player._read = function(n) { // Speakerモジュールで新しいサンプルデータが必要になったら呼び出されるコールバック関数 n:バイト数
  if(g_samples.length>=9600) {
    let loopNum=Math.floor(g_samples.length/AEC3_SAMPLES_PER_FRAME);
    if(loopNum>10) loopNum=10;
    const toplay = new Uint8Array(AEC3_SAMPLES_PER_FRAME*2*loopNum);
    const dv=new DataView(toplay.buffer);
    const rec=new Int16Array(AEC3_SAMPLES_PER_FRAME);
    const st=new Date().getTime();
    for(let j=0;j<loopNum;j++) {      
      for(let i=0;i<AEC3_SAMPLES_PER_FRAME;i++) {
        rec[i]=g_samples.shift();
      }
      if(aec3Wrapper.initialized) {
        aec3Wrapper.update_rec_frame_wrapped(rec);
        const ref=new Int16Array(AEC3_SAMPLES_PER_FRAME);
        for(let i=0;i<AEC3_SAMPLES_PER_FRAME;i++) {
          ref[i]=this.ref.shift();
        }
        aec3Wrapper.update_ref_frame_wrapped(ref);
        const processed=new Int16Array(AEC3_SAMPLES_PER_FRAME);
        for(let i=0;i<AEC3_SAMPLES_PER_FRAME;i++) processed[i]=123;
        aec3Wrapper.process_wrapped(80,processed,1);
        g_play_max_sample=0;
        for(let i=0;i<AEC3_SAMPLES_PER_FRAME;i++) {
          const sample=processed[i];
          dv.setInt16((j*AEC3_SAMPLES_PER_FRAME+i)*2,sample,true);
          this.ref.push(sample);
          if(sample>g_play_max_sample)g_play_max_sample=sample;
        }
      } else {
        console.log("aec3 is not initialized yet");
      }
    }
    const et=new Date().getTime();
    g_enh=aec3Wrapper.get_metrics_echo_return_loss_enhancement();
    this.push(toplay);
  } else {
    console.log("need more samples!");
    const sampleNum=n/2;
    const toplay = new Uint8Array(n);
    const dv=new DataView(toplay.buffer);
    for(let i=0;i<sampleNum;i++) {
      const sample=0;
      dv.setInt16(i*2,sample,true);
      this.ref.push(sample);
    }
    this.push(toplay);
  }
}

const spk=new Speaker({ 
  channels: 1, // チャンネル数は1(モノラル)
  bitDepth: 16, // サンプリングデータのビット数は16 (デフォルトはリトルエンディアン)
  sampleRate: FREQ, // サンプリングレート(Hz)
});

player.pipe(spk); 

setInterval(function() {
  console.log("rec:",getVolumeBar(g_rec_max_sample),
              "play:",getVolumeBar(g_play_max_sample),
              "buffer:",g_samples.length,
              "Enhance:",getVolumeBar(g_enh*2000));
},50);
