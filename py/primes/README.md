# Primes MCP Server

このプロジェクトは、Model Context Protocol (MCP) を使用して素数計算機能を提供するサーバーです。

## 機能

このMCPサーバーは以下の機能を提供します：

1. **count_primes**: 指定された上限までの素数の数をカウントします
2. **list_primes**: 指定された範囲内のすべての素数をリストアップします
3. **is_prime_number**: 指定された数が素数かどうかを判定します
4. **find_nth_prime**: n番目の素数を見つけます

## 使用方法

### サーバーの起動

```bash
python primes.py
```

### MCPクライアントからの利用

MCPクライアントを使用して、以下のようにツールを呼び出すことができます：

```python
# count_primesツールの呼び出し例
result = await client.call("count_primes", limit=1000)
print(f"1000までの素数の数: {result}")

# list_primesツールの呼び出し例
result = await client.call("list_primes", start=10, end=50)
print(f"10から50までの素数: {result}")

# is_prime_numberツールの呼び出し例
result = await client.call("is_prime_number", number=17)
print(result)

# find_nth_primeツールの呼び出し例
result = await client.call("find_nth_prime", n=10)
print(result)
```

## 依存関係

- Python 3.13以上
- MCP 1.3.0以上
