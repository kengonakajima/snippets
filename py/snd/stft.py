import wave as wave
import pyroomacoustics as pa
import numpy as np
import matplotlib.pyplot as plt
import scipy.signal as sp

pa.datasets.CMUArcticCorpus(basedir="./CMU_ARCTIC",download=True,speaker=["aew","axb"])

#sample_wave_file="./CMU_ARCTIC/cmu_us_aew_arctic/wav/arctic_a0001.wav"

sample_wave_file="./howling.wav"

wav=wave.open(sample_wave_file)

print("hz:",wav.getframerate())
print("byte:",wav.getsampwidth())
print("samplenum:",wav.getnframes())
print("ch:",wav.getnchannels())

data=wav.readframes(wav.getnframes())
data=np.frombuffer(data,dtype=np.int16)
data=data/np.iinfo(np.int16).max

f,t,stft_data=sp.stft(data,fs=wav.getframerate(),window="hann",nperseg=512,noverlap=256)

print("shape:", np.shape(stft_data))
print("hz:",f)
print("sec:",t)

print("data:",stft_data)


fig=plt.figure(figsize=(10,4))

spectrum,freqs,t,im=plt.specgram(data,NFFT=512,noverlap=512/16*15,Fs=wav.getframerate(),cmap="gray")

fig.colorbar(im).set_label("intensity db")
plt.xlabel("sec")
plt.ylabel("hz")
plt.savefig("./spectrogram.png")
plt.show()


# stft_data[50:,:]=0
stft_data[:50,:]=0

t,data_post=sp.istft(stft_data,fs=wav.getframerate(), window="hann",nperseg=512,noverlap=256)
data_post=data_post*np.iinfo(np.int16).max
data_post=data_post.astype(np.int16)
print(data_post)


wave_out=wave.open("post.wav","w")
wave_out.setnchannels(1)
wave_out.setsampwidth(2)
wave_out.setframerate(wav.getframerate())
wave_out.writeframes(data_post)
wave_out.close()

