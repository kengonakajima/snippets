import wave as wave
import pyroomacoustics as pa
import numpy as np
import matplotlib.pyplot as plt

n_sample=40000
data=np.random.normal(scale=0.01,size=n_sample)
data_scale_adjust=data*np.iinfo(np.int16).max
data_scale_adjust=data_scale_adjust.astype(np.int16)

wave_out=wave.open("hoge.wav","w")
wave_out.setnchannels(1)
wave_out.setsampwidth(2)
wave_out.setframerate(8000)
wave_out.writeframes(data_scale_adjust)
wave_out.close()
