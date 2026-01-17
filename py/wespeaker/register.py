import wespeaker
import numpy as np
import sounddevice as sd
import scipy.io.wavfile as wavfile
import sys
import threading

duration = 5  # 録音秒数
sample_rate = 16000
audio_buffer = []
voice_frames = 0  # 音声が検出されたフレーム数
total_frames = 0
voice_threshold = 0.02  # 音声検出しきい値

def audio_callback(indata, frames, time, status):
    global voice_frames, total_frames
    audio_buffer.append(indata.copy())

    # RMS（音量レベル）を計算
    rms = np.sqrt(np.mean(indata**2))
    total_frames += 1

    # 音声検出
    if rms > voice_threshold:
        voice_frames += 1

    # 音量バー表示
    bar_length = int(rms * 200)
    bar = '#' * min(bar_length, 40)
    voice_ratio = (voice_frames / total_frames * 100) if total_frames > 0 else 0

    # リアルタイム表示
    sys.stdout.write(f'\r音量: [{bar:<40}] | 音声: {voice_ratio:5.1f}%')
    sys.stdout.flush()

# モデルをロード
print("モデルをロード中...")
model = wespeaker.load_model('campplus')

# マイク録音
print(f"\n{duration}秒間、マイクに向かって話してください...")
print("録音開始!\n")

with sd.InputStream(samplerate=sample_rate, channels=1, dtype='float32', callback=audio_callback):
    sd.sleep(int(duration * 1000))

print("\n\n録音終了")

# 音声データを結合
audio = np.concatenate(audio_buffer)
voice_ratio = (voice_frames / total_frames * 100) if total_frames > 0 else 0
print(f"音声区間: {voice_ratio:.1f}%")

if voice_ratio < 20:
    print("警告: 音声が少なすぎます。もう一度録音してください。")
    sys.exit(1)

# 一時ファイルに保存
audio_int16 = (audio * 32767).astype(np.int16)
wavfile.write('temp_register.wav', sample_rate, audio_int16)

# 埋め込み抽出
print("特徴ベクトルを抽出中...")
embedding = model.extract_embedding('temp_register.wav')

# ファイルに保存
np.save('my_voice_embedding.npy', embedding)
print(f"登録完了: my_voice_embedding.npy")
print(f"Embedding shape: {embedding.shape}")
