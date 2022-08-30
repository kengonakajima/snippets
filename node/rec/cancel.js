const recorder = require('node-record-lpcm16'); // nodeモジュールを読み込む
const fs=require("fs");

const FREQ=48000;
const AEC3_SAMPLES_PER_FRAME=FREQ/100;

const g_mic_buf=[];

const RINGBUF_LEN=FREQ*2;
const g_samples_ring=new Int16Array(RINGBUF_LEN); // lpcm16

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
  
/*
      AEC3_API void AEC3_API_CC aec3_enable_debug_recording();    
    AEC3_API void AEC3_API_CC aec3_update_ref_frame(int16_t *samples, int32_t num);
    AEC3_API void AEC3_API_CC aec3_update_rec_frame(int16_t *samples, int32_t num);
    AEC3_API void AEC3_API_CC aec3_process(int32_t delay_ms, int16_t *outsamples, int32_t num, int32_t use_ns);
    AEC3_API void AEC3_API_CC aec3_get_metrics(double *echo_return_loss, double *echo_return_loss_enhancement, int32_t *delay_ms );
    AEC3_API double AEC3_API_CC aec3_get_metrics_echo_return_loss();
    AEC3_API double AEC3_API_CC aec3_get_metrics_echo_return_loss_enhancement();
    AEC3_API int32_t AEC3_API_CC aec3_get_metrics_delay_ms();
    AEC3_API int32_t AEC3_API_CC aec3_get_noise_suppressor_diff();
    AEC3_API float AEC3_API_CC aec3_get_loopback_device_level();
    AEC3_API int32_t AEC3_API_CC aec3_get_loopback_buffer_used();
*/
  
  aec3Wrapper.debug_print();
  aec3Wrapper.init(4,0);
  aec3Wrapper.initialized=true;  
}


///////////
// recording
let g_rec_cnt=0;
recorder
  .record({
    sampleRate: FREQ, // マイクデバイスのサンプリングレートを指定
    channels: 1,  // チャンネル数を指定(モノラル)              
    recordProgram: 'rec', // 録音用のバックエンドプログラム名を指定
  })
  .stream()
  .on('error', console.error) // エラーが起きたときにログを出力する
  .on('data', function(data) { // マイクからデータを受信する無名コールバック関数
    const vol = Math.abs(data.readInt16LE());  // 配列の先頭のサンプリングデータをひとつ読み込み、音量を得る
    const ntimes = vol / 512; // 音量が0~32767の値で得られるので512で割る(0~63)
    const bar = "*".repeat(ntimes); // アスタリスク文字を、音量に応じて0~63回繰り返す
    const sampleNum=data.length/2;
    for(let i=0;i<sampleNum;i++) {
      const sample=data.readInt16LE(i*2);
      g_mic_buf.push(sample);
    }
    for(;;) {
      if(g_mic_buf.length<AEC3_SAMPLES_PER_FRAME) break;
      for(let i=0;i<AEC3_SAMPLES_PER_FRAME;i++) {
        const sample=g_mic_buf.shift();
        g_samples_ring[g_rec_cnt%RINGBUF_LEN]=sample;
        g_rec_cnt++;      
      }
    }
    console.log("rec: volume:", bar, "total:",g_rec_cnt,"len:",data.length);//,g_samples.slice(0,8).join(","));
  });
console.log('Listening, press Ctrl+C to stop.');


/////////////////////
// playing

const Readable=require("stream").Readable; 
const Speaker=require("speaker");

let g_play_cnt=0;

const player=new Readable(); // 
player.t=0;    // 音波を生成する際の時刻カウンター
player.ref=[];
player._read = function(n) { // Speakerモジュールで新しいサンプルデータが必要になったら呼び出されるコールバック関数 n:バイト数
//  console.log("play:",g_play_cnt,"diff:",(g_rec_cnt-g_play_cnt),"n:",n);
  const available=g_rec_cnt-g_play_cnt;
  const keep=FREQ/5;
  const canPlay=available-keep;
  console.log("available:",available,"canPlay:",canPlay,"ref.len:",this.ref.length);
  if(canPlay>=FREQ/10) {
    let loopNum=Math.floor(available/AEC3_SAMPLES_PER_FRAME);
    if(loopNum>10) loopNum=10;
    const toplay = new Uint8Array(AEC3_SAMPLES_PER_FRAME*2*loopNum);
    const dv=new DataView(toplay.buffer);
    const rec=new Int16Array(AEC3_SAMPLES_PER_FRAME);
    const st=new Date().getTime();
    for(let j=0;j<loopNum;j++) {      
      for(let i=0;i<AEC3_SAMPLES_PER_FRAME;i++) {
        rec[i]=g_samples_ring[g_play_cnt%RINGBUF_LEN];
        g_play_cnt++;      
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
        for(let i=0;i<AEC3_SAMPLES_PER_FRAME;i++) {
          const sample=processed[i];
          dv.setInt16((j*AEC3_SAMPLES_PER_FRAME+i)*2,sample,true);
          this.ref.push(sample);
        }
      } else {
        console.log("aec3 is not initialized yet");
      }
    }
    const et=new Date().getTime();
    const enh=aec3Wrapper.get_metrics_echo_return_loss_enhancement();
    console.log("ENH:",enh, "toplay:",toplay.length,"sample:",toplay[0],"aectime:",et-st);
    this.push(toplay);
  } else {
    console.log("need more samples!");
    // 音が足りないのでピー音
    const toGen=FREQ/10;
    const toplay = new Uint8Array(toGen*2);
    const dv=new DataView(toplay.buffer);         
    for(let i=0;i<toGen;i++) {
      const sampleNoise=-50+Math.random()*100;
      dv.setInt16(i*2,sampleNoise,true);
      this.ref.push(sampleNoise);
    }
    this.push(toplay);
  }
    
/*    
    if(aec3Wrapper.initialized) {
      const frameNum=Math.floor(sampleNum/AEC3_SAMPLES_PER_FRAME);
      for(let fi=0;fi<frameNum;fi++) {
        const toplay16=new Int16Array(sampleNum);
        for(let i=0;i<sampleNum;i++) toplay16[i]=dv.getInt16(i,true);

        
      }

      //     aec3_update_ref_frame(reinterpret_cast<int16_t*>(ref_tmp),AEC3_SAMPLES_PER_FRAME);
      //  aec3_update_rec_frame(reinterpret_cast<int16_t*>(aec_tmp),AEC3_SAMPLES_PER_FRAME);
      //aec3_process(80,reinterpret_cast<int16_t*>(aec_tmp),AEC3_SAMPLES_PER_FRAME,0);
    }
*/
/*  
  } else {
    console.log("need more samples in ring buffer, play sine wave.",sampleNum);

  */
}

const spk=new Speaker({ 
  channels: 1, // チャンネル数は1(モノラル)
  bitDepth: 16, // サンプリングデータのビット数は16 (デフォルトはリトルエンディアン)
  sampleRate: FREQ, // サンプリングレート(Hz)
});

player.pipe(spk); 

