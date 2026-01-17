import wespeaker
import numpy as np
import sounddevice as sd
import scipy.io.wavfile as wavfile
import sys
import threading
import os

duration = 5  # 録音秒数
sample_rate = 16000
voice_threshold = 0.02  # 音声検出しきい値
takes = 3
scripts = [
    "今日は風が気持ちよくて、窓の外から鳥の声が聞こえます。",
    "この部屋は静かで、時計の音が小さく響いています。",
    "ゆっくり息を吸って、落ち着いた声で話してみます。",
]

# 保存先
embedding_dir = "embeddings"
name = input("登録する名前を入力してください: ").strip()
if not name:
    print("エラー: 名前が空です。")
    sys.exit(1)
os.makedirs(embedding_dir, exist_ok=True)
save_path = os.path.join(embedding_dir, f"{name}.npy")

def to_numpy_embedding(x):
    if hasattr(x, "numpy"):
        x = x.numpy()
    return np.squeeze(np.array(x, dtype=np.float32))

def record_take(model, script_text, take_index):
    while True:
        audio_buffer = []
        voice_frames = 0
        total_frames = 0
        started = False

        def audio_callback(indata, frames, time, status):
            nonlocal voice_frames, total_frames, started
            audio_buffer.append(indata.copy())

            # RMS（音量レベル）を計算
            rms = np.sqrt(np.mean(indata**2))
            if rms > voice_threshold:
                if not started:
                    started = True
            if started:
                total_frames += 1

            # 音声検出
            if rms > voice_threshold and started:
                voice_frames += 1

            # 音量バー表示
            bar_length = int(rms * 200)
            bar = "#" * min(bar_length, 40)
            voice_ratio = (voice_frames / total_frames * 100) if total_frames > 0 else 0

            # リアルタイム表示
            sys.stdout.write(f"\r音量: [{bar:<40}] | 音声: {voice_ratio:5.1f}%")
            sys.stdout.flush()

        print(f"\nテイク{take_index + 1}/{takes}")
        print(f"台本: {script_text}")
        print(f"\n{duration}秒間、マイクに向かって話してください...")
        print("録音開始!\n")

        with sd.InputStream(samplerate=sample_rate, channels=1, dtype="float32", callback=audio_callback):
            sd.sleep(int(duration * 1000))

        print("\n\n録音終了")

        # 音声データを結合
        audio = np.concatenate(audio_buffer)
        voice_ratio = (voice_frames / total_frames * 100) if total_frames > 0 else 0
        print(f"音声区間: {voice_ratio:.1f}%")

        if voice_ratio < 20:
            print("警告: 音声が少なすぎます。このテイクをやり直します。")
            continue

        # 一時ファイルに保存
        audio_int16 = (audio * 32767).astype(np.int16)
        wavfile.write("temp_register.wav", sample_rate, audio_int16)

        # 埋め込み抽出
        print("特徴ベクトルを抽出中...")
        embedding = model.extract_embedding("temp_register.wav")
        return to_numpy_embedding(embedding)

# モデルをロード
print("モデルをロード中...")
model = wespeaker.load_model('campplus')

embeddings = []
for i in range(takes):
    embeddings.append(record_take(model, scripts[i], i))

mean_embedding = np.mean(np.stack(embeddings, axis=0), axis=0)

# ファイルに保存
np.save(save_path, mean_embedding)
print(f"登録完了: {save_path}")
print(f"Embedding shape: {mean_embedding.shape}")
