import sounddevice as sd
import numpy as np
import time

sample_rate = 16000
rms_max = 0.0


def callback(indata, frames, time_info, status):
    global rms_max
    rms = float(np.sqrt(np.mean(indata**2)))
    if rms > rms_max:
        rms_max = rms
    bar = "#" * min(int(rms * 200), 40)
    print(f"\r音量: [{bar:<40}] rms={rms:.4f}", end="")


with sd.InputStream(samplerate=sample_rate, channels=1, dtype="float32", callback=callback):
    print("5秒間話してください...")
    time.sleep(5)

print()
print("max_rms:", rms_max)
print("default input:", sd.default.device)
