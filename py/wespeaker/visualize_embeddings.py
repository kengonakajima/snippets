import os
import sys

import numpy as np
import matplotlib.pyplot as plt
from sklearn.decomposition import PCA


def load_embeddings(embedding_dir):
    if not os.path.isdir(embedding_dir):
        print(f"エラー: {embedding_dir} が見つかりません。")
        sys.exit(1)

    labels = []
    vectors = []
    for name in sorted(os.listdir(embedding_dir)):
        if not name.endswith(".npy"):
            continue
        path = os.path.join(embedding_dir, name)
        emb = np.load(path)
        emb = np.squeeze(np.array(emb, dtype=np.float32))
        if emb.ndim != 1:
            print(f"エラー: {path} の次元が想定外です: ndim={emb.ndim}")
            sys.exit(1)
        labels.append(os.path.splitext(name)[0])
        vectors.append(emb)

    if not vectors:
        print(f"エラー: {embedding_dir} に .npy がありません。")
        sys.exit(1)

    dim = vectors[0].shape[0]
    for i, v in enumerate(vectors):
        if v.shape[0] != dim:
            print(f"エラー: 次元が不一致です: {labels[i]} dim={v.shape[0]} != {dim}")
            sys.exit(1)

    return labels, np.stack(vectors, axis=0)


labels, X = load_embeddings("embeddings")
print(f"人数: {len(labels)}")
print(f"次元: {X.shape[1]}")
print("名前:", ", ".join(labels))

norms = np.linalg.norm(X, axis=1)
if np.any(norms == 0):
    print("エラー: ゼロベクトルのembeddingがあります。")
    sys.exit(1)

X_norm = X / norms[:, None]
sim = X_norm @ X_norm.T

plt.figure(figsize=(6, 5))
plt.imshow(sim, vmin=-1.0, vmax=1.0, cmap="coolwarm")
plt.colorbar(label="cosine similarity")
plt.xticks(range(len(labels)), labels, rotation=45, ha="right")
plt.yticks(range(len(labels)), labels)
plt.title("Embedding Cosine Similarity")
plt.tight_layout()
plt.savefig("embeddings_similarity.png", dpi=150)
print("保存: embeddings_similarity.png")

if len(labels) < 2:
    print("人数が2未満のためPCA散布図は作成しません。")
    sys.exit(0)

pca = PCA(n_components=2)
coords = pca.fit_transform(X)

plt.figure(figsize=(6, 5))
plt.scatter(coords[:, 0], coords[:, 1], s=60)
for i, label in enumerate(labels):
    plt.text(coords[i, 0], coords[i, 1], label, fontsize=9, ha="left", va="bottom")
plt.title("PCA (2D)")
plt.xlabel("PC1")
plt.ylabel("PC2")
plt.tight_layout()
plt.savefig("embeddings_pca.png", dpi=150)
print("保存: embeddings_pca.png")
