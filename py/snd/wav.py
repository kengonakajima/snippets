import wave as wave
import pyroomacoustics as pa
import numpy as np
import matplotlib.pyplot as plt

pa.datasets.CMUArcticCorpus(basedir="./CMU_ARCTIC",download=True,speaker=["aew","axb"])

sample_wave_file="./CMU_ARCTIC/cmu_us_aew_arctic/wav/arctic_a0001.wav"

wav=wave.open(sample_wave_file)

print("hz:",wav.getframerate())
print("byte:",wav.getsampwidth())
print("samplenum:",wav.getnframes())
print("ch:",wav.getnchannels())

data=wav.readframes(wav.getnframes())
data=np.frombuffer(data,dtype=np.int16)
data=data/np.iinfo(np.int16).max

#data=np.random.normal(size=wav.getnframes())

print(data)

x=np.array(range(wav.getnframes()))/wav.getframerate()

print(x)

plt.figure(figsize=(10,4))
plt.xlabel("Time[sec]")
plt.ylabel("Value[-1,1]")
plt.plot(x,data)
plt.savefig("./waveform.png")
plt.show()

wav.close()
