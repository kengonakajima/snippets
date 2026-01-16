#!/usr/bin/env python3
"""RLM (Recursive Language Model) - LongBench v2 評価用"""

import argparse
import json
import os
import re
from pathlib import Path

import openai

# OpenRouter設定
DEFAULT_MODEL = "openai/gpt-oss-120b"
OPENROUTER_BASE_URL = "https://openrouter.ai/api/v1"

# グローバル設定
client = None
model = DEFAULT_MODEL


# init_client()
# 引数: なし
# 戻り値: なし（グローバル変数clientを設定）
# 説明: OPENROUTER_API_KEY環境変数からAPIキーを取得し、OpenAIクライアントを初期化
def init_client():
    """OpenRouter クライアントを初期化"""
    global client
    api_key = os.environ.get("OPENROUTER_API_KEY")
    if not api_key:
        raise ValueError("OPENROUTER_API_KEY 環境変数を設定してください")
    client = openai.OpenAI(
        base_url=OPENROUTER_BASE_URL,
        api_key=api_key,
    )


# call_llm(prompt, system, label)
# 引数:
#   prompt: str - LLMに送るユーザーメッセージ
#   system: str - システムメッセージ（省略可）
#   label: str - ログ出力用のラベル（省略可、空文字ならログ出力なし）
# 戻り値: str - LLMの応答テキスト
# 説明: OpenRouter APIを呼び出し、応答を取得。labelが指定されていればプロンプトと応答をログ出力
def call_llm(prompt: str, system: str = "", label: str = "") -> str:
    """LLMを呼び出して応答を取得"""
    messages = []
    if system:
        messages.append({"role": "system", "content": system})
    messages.append({"role": "user", "content": prompt})

    if label:
        print(f"\n    [LLM:{label}] プロンプト:")
        for line in prompt.split("\n"):
            print(f"      | {line}")

    response = client.chat.completions.create(
        model=model,
        messages=messages,
    )
    content = response.choices[0].message.content

    if label:
        print(f"    [LLM:{label}] 応答:")
        for line in content.split("\n"):
            print(f"      > {line}")

    return content


# load_problem(problem_id)
# 引数:
#   problem_id: str - 問題番号（例: "000", "002"）
# 戻り値: dict - 問題データ（_id, domain, question, choice_A〜D, answer, context等を含む）
# 説明: longbench_v2/ディレクトリから指定された問題番号のJSONファイルを読み込む
def load_problem(problem_id: str) -> dict:
    """問題を読み込む"""
    data_dir = Path(__file__).parent / "longbench_v2"

    # 問題番号でファイルを検索
    for f in data_dir.glob(f"{problem_id}_*.json"):
        with open(f, "r", encoding="utf-8") as fp:
            return json.load(fp)

    raise FileNotFoundError(f"問題 {problem_id} が見つかりません")


# search(query, context, top_k)
# 引数:
#   query: str - 検索クエリ（スペース区切りのキーワード）
#   context: str - 検索対象の長文テキスト（改行区切り）
#   top_k: int - 返すヒット数の上限（デフォルト10）
# 戻り値: tuple(list[dict], list[str], int) - (ヒットのリスト, キーワードリスト, 総行数)
# 説明: クエリをキーワードに分割し、各行でマッチしたキーワード数をスコアとしてTop-Kを返す
def search(query: str, context: str, top_k: int = 10) -> tuple:
    """コンテキスト内をキーワード検索し、Top-K行を返す"""
    lines = context.split("\n")
    hits = []

    # クエリをキーワードに分割
    keywords = query.lower().split()
    total_lines = len(lines)

    for line_no, line in enumerate(lines):
        line_lower = line.lower()
        # マッチしたキーワード数をスコアに
        score = sum(1 for kw in keywords if kw in line_lower)
        if score > 0:
            hits.append({
                "line_no": line_no,
                "score": score,
                "text": line[:500],  # 長すぎる行は切り詰め
                "context": "\n".join(lines[max(0, line_no-2):line_no+3])[:1000]
            })

    # スコア順でソートしてTop-K
    hits.sort(key=lambda x: x["score"], reverse=True)
    return hits[:top_k], keywords, total_lines


# init_state(task)
# 引数:
#   task: dict - 問題データ（question, choice_A〜D等を含む）
# 戻り値: dict - 初期状態 {question, choices, evidence, hypothesis, iteration}
# 説明: RLMループの初期状態を生成。evidenceは空リスト、iterationは0で開始
def init_state(task: dict) -> dict:
    """状態を初期化"""
    return {
        "question": task["question"],
        "choices": {
            "A": task["choice_A"],
            "B": task["choice_B"],
            "C": task["choice_C"],
            "D": task["choice_D"],
        },
        "evidence": [],  # 収集した根拠
        "hypothesis": None,  # 現在の仮説 (A/B/C/D)
        "iteration": 0,
    }


# propose_query(state)
# 引数:
#   state: dict - 現在の状態 {question, choices, evidence, ...}
# 戻り値: str - 次に検索すべきキーワード（1行）
# 説明: LLMに質問・選択肢・これまでの根拠を提示し、次の検索クエリを生成させる
def propose_query(state: dict) -> str:
    """LLMで次の検索クエリを生成"""
    prompt = f"""質問に答えるために、文書を検索します。
次に検索すべきキーワードを1行で出力してください。

質問: {state["question"]}

選択肢:
A: {state["choices"]["A"]}
B: {state["choices"]["B"]}
C: {state["choices"]["C"]}
D: {state["choices"]["D"]}

これまでに見つけた根拠:
{chr(10).join(state["evidence"][-5:]) if state["evidence"] else "(なし)"}

検索キーワード:"""

    return call_llm(prompt, label="propose_query").strip()


# extract_evidence(hits, task, state)
# 引数:
#   hits: list[dict] - search()の結果（各要素は {line_no, score, text, context}）
#   task: dict - 問題データ（questionを参照）
#   state: dict - 現在の状態（未使用だが将来の拡張用）
# 戻り値: list[str] - 抽出された根拠のリスト（最大5件）。役立つ情報がなければ空リスト
# 説明: LLMに検索結果を提示し、質問に答えるのに役立つ情報を箇条書きで抽出させる
def extract_evidence(hits: list, task: dict, state: dict) -> list:
    """検索結果から有用な情報を抽出"""
    if not hits:
        return []

    hits_text = "\n---\n".join([
        f"[行{h['line_no']}] {h['context']}" for h in hits[:5]
    ])

    prompt = f"""以下の検索結果から、質問に答えるのに役立つ情報を抽出してください。
役立つ情報があれば、1行ずつ箇条書きで出力してください。
役立つ情報がなければ「なし」と出力してください。

質問: {task["question"]}

検索結果:
{hits_text}

役立つ情報:"""

    response = call_llm(prompt, label="extract_evidence")
    if "なし" in response:
        return []

    # 箇条書きを抽出
    lines = [line.strip().lstrip("-・").strip()
             for line in response.split("\n")
             if line.strip() and not line.strip().startswith("#")]
    return lines[:5]  # 最大5件


# update_state(state, delta)
# 引数:
#   state: dict - 現在の状態 {question, choices, evidence, hypothesis, iteration}
#   delta: list[str] - 新たに抽出された根拠のリスト
# 戻り値: tuple(dict, dict) - (更新後の状態, 更新詳細 {added, duplicates, truncated})
# 説明: iterationを+1し、deltaの根拠をevidenceに追加（重複排除、最大20件まで保持）
def update_state(state: dict, delta: list) -> tuple:
    """状態を更新"""
    state = state.copy()
    state["evidence"] = state["evidence"].copy()
    state["iteration"] += 1

    # 新しい根拠を追加（重複排除）
    added = 0
    duplicates = 0
    for item in delta:
        if item and item not in state["evidence"]:
            state["evidence"].append(item)
            added += 1
        elif item:
            duplicates += 1

    # 根拠が多すぎたら古いものを削除
    truncated = 0
    if len(state["evidence"]) > 20:
        truncated = len(state["evidence"]) - 20
        state["evidence"] = state["evidence"][-20:]

    update_info = {"added": added, "duplicates": duplicates, "truncated": truncated}
    return state, update_info


# converged(state)
# 引数:
#   state: dict - 現在の状態 {evidence, iteration, ...}
# 戻り値: bool - 収束したらTrue、続行ならFalse
# 説明: iteration >= 10 または evidence >= 10件 なら収束と判定
def converged(state: dict) -> bool:
    """収束判定"""
    # 最大イテレーション数
    if state["iteration"] >= 10:
        return True
    # 十分な根拠が集まったら収束
    if len(state["evidence"]) >= 10:
        return True
    return False


# finalize(state)
# 引数:
#   state: dict - 現在の状態 {question, choices, evidence, ...}
# 戻り値: str - 最終回答（"A", "B", "C", "D" のいずれか）
# 説明: LLMに質問・選択肢・累積根拠を提示し、最終回答を1文字で出力させる
def finalize(state: dict) -> str:
    """最終回答を決定"""
    evidence_text = "\n".join(f"- {e}" for e in state["evidence"])

    prompt = f"""以下の情報に基づいて、質問に対する回答を選んでください。
A, B, C, D のいずれか1文字のみを出力してください。

質問: {state["question"]}

選択肢:
A: {state["choices"]["A"]}
B: {state["choices"]["B"]}
C: {state["choices"]["C"]}
D: {state["choices"]["D"]}

収集した根拠:
{evidence_text if evidence_text else "(なし)"}

回答:"""

    response = call_llm(prompt, label="finalize").strip().upper()

    # A/B/C/D を抽出
    for c in response:
        if c in "ABCD":
            return c
    return "A"  # フォールバック


# rlm_loop(task, s_max)
# 引数:
#   task: dict - 問題データ {question, choice_A〜D, answer, context, ...}
#   s_max: int - 最大イテレーション数（ループの上限）
# 戻り値: str - 最終回答（"A", "B", "C", "D" のいずれか）
# 説明: RLMのメインループ。init_state → [propose_query → search → extract_evidence → update_state → converged?] × N → finalize
def rlm_loop(task: dict, s_max: int) -> str:
    """RLMループを実行"""
    state = init_state(task)
    context = task["context"]
    context_lines = len(context.split("\n"))
    context_chars = len(context)

    print(f"問題: {task['question']}")
    print(f"正解: {task['answer']}")
    print(f"コンテキスト: {context_lines:,}行, {context_chars:,}文字")
    print("-" * 50)

    for s in range(s_max):
        print(f"\n{'='*50}")
        print(f"[イテレーション {s + 1}/{s_max}]")
        print(f"  state.iteration: {state['iteration']}")
        print(f"  state.hypothesis: {state['hypothesis']}")
        print(f"  state.evidence: {len(state['evidence'])}件")
        if state["evidence"]:
            print(f"    最新3件:")
            for i, e in enumerate(state["evidence"][-3:]):
                idx = len(state["evidence"]) - 3 + i + 1
                print(f"      [{idx}] {e[:60]}...")

        # 検索クエリを生成
        query = propose_query(state)
        print(f"  生成されたクエリ: {query}")

        # 検索実行
        hits, keywords, total_lines = search(query, context)
        print(f"  検索キーワード: {keywords}")
        print(f"  検索対象: {total_lines:,}行")
        print(f"  ヒット: {len(hits)}件")
        for i, h in enumerate(hits[:3]):
            print(f"    [{i+1}] 行{h['line_no']} (スコア:{h['score']}): {h['text'][:60]}...")

        # 根拠抽出
        delta = extract_evidence(hits, task, state)
        print(f"  抽出された根拠: {len(delta)}件")
        for d in delta:
            print(f"    - {d[:80]}...")

        # 状態更新
        prev_evidence_count = len(state["evidence"])
        prev_iteration = state["iteration"]
        state, update_info = update_state(state, delta)
        print(f"  状態更新:")
        print(f"    iteration: {prev_iteration} -> {state['iteration']}")
        print(f"    evidence: {prev_evidence_count} -> {len(state['evidence'])} (追加:{update_info['added']}, 重複:{update_info['duplicates']}, 切り捨て:{update_info['truncated']})")

        # 収束判定（毎回表示）
        is_converged = converged(state)
        print(f"  収束判定:")
        print(f"    iteration >= 10? {state['iteration']} >= 10 -> {state['iteration'] >= 10}")
        print(f"    evidence >= 10? {len(state['evidence'])} >= 10 -> {len(state['evidence']) >= 10}")
        print(f"    結果: {'収束' if is_converged else '続行'}")

        if is_converged:
            reason = "最大イテレーション到達" if state["iteration"] >= 10 else "十分な根拠収集"
            print(f"\n*** 収束: {reason} ***")
            break

    # 最終回答
    print(f"\n{'='*50}")
    print("[最終回答フェーズ]")
    print(f"  最終state:")
    print(f"    iteration: {state['iteration']}")
    print(f"    hypothesis: {state['hypothesis']}")
    print(f"    evidence: {len(state['evidence'])}件")
    print(f"  累積根拠一覧:")
    for i, e in enumerate(state["evidence"]):
        print(f"    {i+1}. {e}")
    answer = finalize(state)

    # hypothesisを更新して表示
    state["hypothesis"] = answer
    print(f"  最終hypothesis: {state['hypothesis']}")

    return answer


# main()
# 引数: なし（コマンドライン引数から取得）
# コマンドライン引数:
#   problem_id: 問題番号（必須、例: "002"）
#   --model: 使用するモデル（省略可、デフォルト: openai/gpt-oss-120b）
#   --s-max: 最大イテレーション数（省略可、デフォルト: 5）
# 説明: エントリーポイント。引数をパースし、問題を読み込み、RLMループを実行して結果を表示
def main():
    global model

    parser = argparse.ArgumentParser(description="RLM - LongBench v2 評価")
    parser.add_argument("problem_id", help="問題番号 (例: 002)")
    parser.add_argument("--model", default=DEFAULT_MODEL, help="使用するモデル")
    parser.add_argument("--s-max", type=int, default=5, help="最大イテレーション数")
    args = parser.parse_args()

    model = args.model

    # クライアント初期化
    init_client()

    # 問題読み込み
    task = load_problem(args.problem_id)
    print(f"問題ID: {task['_id']}")
    print(f"ドメイン: {task['domain']} / {task['sub_domain']}")
    print(f"難易度: {task['difficulty']}, 長さ: {task['length']}")
    print(f"モデル: {model}")
    print("=" * 50)

    # RLMループ実行
    answer = rlm_loop(task, args.s_max)

    # 結果表示
    print("=" * 50)
    print(f"RLM回答: {answer}")
    print(f"正解: {task['answer']}")
    print(f"結果: {'正解!' if answer == task['answer'] else '不正解'}")


if __name__ == "__main__":
    main()
