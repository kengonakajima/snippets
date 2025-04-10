
from transformers import AutoModelForCausalLM, AutoTokenizer
import torch

# デバイスの設定
device = torch.device("mps")

# モデルとトークナイザーの読み込み
# phi-4は時間かかりすぎた
model = AutoModelForCausalLM.from_pretrained("microsoft/phi-2", device_map="auto", torch_dtype="auto")
tokenizer = AutoTokenizer.from_pretrained("microsoft/phi-2", trust_remote_code=True)

# 推論の実行
def generate_text(prompt, max_length=100):
    inputs = tokenizer(prompt, return_tensors="pt", return_attention_mask=True)
    # 入力をMPSデバイスに移動
    inputs = {k: v.to(device) for k, v in inputs.items()}
    
    outputs = model.generate(
        inputs['input_ids'],
        attention_mask=inputs['attention_mask'],
        max_length=max_length,
        num_return_sequences=1,
        temperature=0.7,
        do_sample=True
    )
    return tokenizer.decode(outputs[0], skip_special_tokens=True)

prompt = "Write a hello world program in Python:"
response = generate_text(prompt)
print(response)


