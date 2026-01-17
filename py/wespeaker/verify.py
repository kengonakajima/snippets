import collections
import os
import sys
import time

import numpy as np
import sounddevice as sd
import scipy.io.wavfile as wavfile
import wespeaker

sample_rate = 16000
window_sec = 3.0
judge_interval = 0.5
voice_threshold = 0.01
similarity_threshold = 0.6

buffer_size = int(window_sec * sample_rate)
audio_ring = collections.deque(maxlen=buffer_size)
latest_rms = 0.0


def audio_callback(indata, frames, time_info, status):
    global latest_rms
    latest_rms = float(np.sqrt(np.mean(indata ** 2)))
    audio_ring.extend(indata[:, 0])


def to_numpy_embedding(x):
    if hasattr(x, "numpy"):
        x = x.numpy()
    return np.squeeze(np.array(x, dtype=np.float32))


print("モデルをロード中...")
model = wespeaker.load_model("campplus")

if not os.path.exists("my_voice_embedding.npy"):
    print("エラー: my_voice_embedding.npy が見つかりません。先に register.py を実行してください。")
    sys.exit(1)

registered_embedding = np.load("my_voice_embedding.npy")
reg_emb = to_numpy_embedding(registered_embedding)
print(f"登録済みembeddingをロード: shape={reg_emb.shape}")

print("\nリアルタイム判定開始 (Ctrl+C で終了)")
print(f"判定間隔: {judge_interval}秒, ウィンドウ: {window_sec}秒")
print("-" * 50)

with sd.InputStream(samplerate=sample_rate, channels=1, dtype="float32", callback=audio_callback):
    last_judge_time = 0.0
    while True:
        now = time.time()
        if now - last_judge_time < judge_interval:
            time.sleep(0.01)
            continue
        last_judge_time = now

        if len(audio_ring) < buffer_size:
            buffered_sec = len(audio_ring) / sample_rate
            bar_length = min(int(latest_rms * 400), 40)
            bar = "#" * bar_length
            print(f"音量: [{bar:<40}] | 類似度: ---- | 判定: 準備中 {buffered_sec:.2f}s")
            continue

        audio = np.array(audio_ring, dtype=np.float32)
        if len(audio) < 400:
            continue

        rms = float(np.sqrt(np.mean(audio ** 2)))
        if rms < voice_threshold:
            bar_length = min(int(latest_rms * 400), 40)
            bar = "#" * bar_length
            print(f"音量: [{bar:<40}] | 類似度: ---- | 判定: 無音")
            continue

        audio = np.clip(audio, -1.0, 1.0)
        audio_int16 = (audio * 32767).astype(np.int16)
        wavfile.write("temp_verify.wav", sample_rate, audio_int16)

        start_time = time.time()
        test_embedding = model.extract_embedding("temp_verify.wav")
        inference_time = time.time() - start_time

        test_emb = to_numpy_embedding(test_embedding)
        similarity = float(
            np.dot(reg_emb, test_emb) / (np.linalg.norm(reg_emb) * np.linalg.norm(test_emb))
        )
        judge = "本人" if similarity >= similarity_threshold else "別人"

        bar_length = min(int(latest_rms * 400), 40)
        bar = "#" * bar_length
        print(
            f"音量: [{bar:<40}] | 類似度: {similarity:5.2f} | 判定: {judge} | 推論: {inference_time:.2f}秒"
        )
