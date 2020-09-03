import wave as wave
import pyroomacoustics as pa
import numpy as np
import matplotlib.pyplot as plt
import sounddevice as sd

sample_wave_file="./CMU_ARCTIC/cmu_us_aew_arctic/wav/arctic_a0001.wav"
wav=wave.open(sample_wave_file)

print("hz:",wav.getframerate())
print("byte:",wav.getsampwidth())
print("samplenum:",wav.getnframes())
print("ch:",wav.getnchannels())

data=wav.readframes(wav.getnframes())
data=np.frombuffer(data,dtype=np.int16)
data=data/np.iinfo(np.int16).max

sd.play(data,wav.getframerate())

print("playing")

status=sd.wait()

print(status)

wav.close()
