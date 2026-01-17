import tiktoken

enc = tiktoken.get_encoding("o200k_base")  # GPT-4o, GPT-5.* 系の基本辞書

vocab = enc._mergeable_ranks  # token → id の辞書

# id → token の表を作る
id_to_token = {v: k for k, v in vocab.items()}

print(len(id_to_token))

with open("tokens.txt", "w", encoding="utf-8") as f:
    for i in range(len(id_to_token)):
        token_bytes = id_to_token[i]
        try:
            token_str = token_bytes.decode("utf-8")
        except:
            token_str = repr(token_bytes)
        f.write(token_str + "\n")

print("tokens.txt に出力しました")

