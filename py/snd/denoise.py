import wave as wave
import pyroomacoustics as pa
import numpy as np
import matplotlib.pyplot as plt
import scipy.signal as sp

pa.datasets.CMUArcticCorpus(basedir="./CMU_ARCTIC",download=True,speaker=["aew","axb"])

sample_wave_file="./CMU_ARCTIC/cmu_us_aew_arctic/wav/arctic_a0001.wav"

wav=wave.open(sample_wave_file)

print("hz:",wav.getframerate())
print("byte:",wav.getsampwidth())
print("samplenum:",wav.getnframes())
print("ch:",wav.getnchannels())

data=wav.readframes(wav.getnframes())
data=np.frombuffer(data,dtype=np.int16)
speech_signal=data
data=data/np.iinfo(np.int16).max

f,t,stft_data=sp.stft(data,fs=wav.getframerate(),window="hann",nperseg=512,noverlap=256)

print("shape:", np.shape(stft_data))
print("hz:",f)
print("sec:",t)
print("data:",stft_data)

n_speech=wav.getnframes()
n_noise_only=40000
n_sample=n_noise_only+n_speech
wgn_signal=np.random.normal(scale=0.04,size=n_sample)
wgn_signal=wgn_signal*np.iinfo(np.int16).max
wgn_signal=wgn_signal.astype(np.int16)

mix_signal=wgn_signal
mix_signal[n_noise_only:]+=speech_signal


f,t,stft_data=sp.stft(mix_signal,fs=wav.getframerate(),window="hann",nperseg=512,noverlap=256)

print("stft",stft_data)
amp=np.abs(stft_data)
print("amp:",amp)
print("maximum:",np.maximum(amp,1.e-20))
phase=stft_data/np.maximum(amp,1.e-20)
print(phase)
sampling_rate=wav.getframerate()
n_noise_only_frame=np.sum(t<(n_noise_only/sampling_rate))
print("n_noise_only_frame",n_noise_only_frame)

p=1.0
alpha=2.0

noise_amp=np.power(np.mean(np.power(amp,p)[:,:n_noise_only_frame],axis=1,keepdims=True), 1./2)

print("noise_amp:",noise_amp)

eps=0.01*np.power(amp,p)

processed_amp=np.power(np.maximum(np.power(amp,p)-alpha*np.power(noise_amp,p),eps), 1./p)

print("processed_amp:",processed_amp)

processed_stft_data=processed_amp*phase

t,processed_data_post=sp.istft(processed_stft_data,fs=wav.getframerate(),window="hann",nperseg=512,noverlap=256)

print("t:",t)

processed_data_post=processed_data_post.astype(np.int16)
print("processed_data_post:",processed_data_post)


wave_out=wave.open("processed_post.wav","w")
wave_out.setnchannels(1)
wave_out.setsampwidth(2)
wave_out.setframerate(wav.getframerate())
wave_out.writeframes(processed_data_post)
wave_out.close()


print("mix_signal:",mix_signal)

mix_out=wave.open("mix.wav","w")
mix_out.setnchannels(1)
mix_out.setsampwidth(2)
mix_out.setframerate(wav.getframerate())
mix_out.writeframes(mix_signal)
mix_out.close()
