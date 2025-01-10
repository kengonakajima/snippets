from transformers import AutoModelForCausalLM, AutoTokenizer
import torch

device = torch.device("mps")

model = AutoModelForCausalLM.from_pretrained(
    "microsoft/phi-2", 
    device_map="auto", 
    torch_dtype=torch.float32  # 明示的に float32 を指定
)
tokenizer = AutoTokenizer.from_pretrained("microsoft/phi-2", trust_remote_code=True)

def generate_text(prompt, max_length=100):
    inputs = tokenizer(prompt, return_tensors="pt", return_attention_mask=True)
    inputs = {k: v.to(device) for k, v in inputs.items()}
    
    outputs = model.generate(
        inputs['input_ids'],
        attention_mask=inputs['attention_mask'],
        max_length=max_length,
        num_return_sequences=1,
        temperature=1.0,      # 温度を1.0に変更
        do_sample=True,
        top_k=50,            # top_kを追加
        top_p=0.9            # top_pを追加
    )
    return tokenizer.decode(outputs[0], skip_special_tokens=True)

prompt = "Write a hello world program in Python:"
#prompt = "日本語のだじゃれを3つ考えて"
response = generate_text(prompt)
print(response)
