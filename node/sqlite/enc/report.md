# SQLite + SQLCipher + sqlite-vec ベンチマーク報告書

## 概要
- `sqlite-vec` を `sqlcipher` からロードして 768 次元ベクトル検索を行う仮想テーブル `vec_data` を構築。
- 以下 2 シナリオで暗号化有無による性能差を比較。
  - **Scenario A (10k_top10)**: 1 万件のデータセットから上位 10 件を検索。
  - **Scenario B (100k_top1000)**: 10 万件のデータセットから上位 1,000 件を検索。
- 各シナリオで平文 DB（`*_plain.db`）と暗号化 DB（`*_encrypted.db`）を生成し、INSERT／SELECT を計測。

## 実行環境
- macOS (Homebrew `sqlcipher` 4.6.1 / SQLite 3.46.1 `sqlcipher --version`)
- `sqlite-vec` バージョン: v0.1.7-alpha.2 (`./sqlite-vec/dist/vec0.dylib`)
- Python 3.12.8
- ベンチマーク実行: `python3 scripts/benchmark.py`

## データ生成
- `scripts/generate_data.py`
  - Scenario A 用: `python3 scripts/generate_data.py --rows 10000 --output-sql data/inserts.sql --output-vector data/test_vector.json`
  - Scenario B 用: `python3 scripts/generate_data.py --rows 100000 --output-sql data/inserts_100k.sql --output-vector data/test_vector_100k.json`
- 各レコードは `embedding float[768]` に JSON ベクトル、`hex_id text` に 32 文字の 16 進 ID を格納。

## 測定方法
1. シナリオごとに `scripts/benchmark.py` が `*_plain.db` / `*_encrypted.db` を再生成。
2. `.load vec0` → `CREATE VIRTUAL TABLE vec_data USING vec0(...)` → 対応する `inserts*.sql` を実行。
   - `BEGIN` / `COMMIT` 付きバッチ挿入。
3. SELECT 計測前に同一クエリを 1 回ウォームアップし、2 回目の実行時間を採用。
4. 結果は `data/benchmark_results.json` に保存。

## 計測結果
### Scenario A: 10,000 rows / Top 10
| ケース | INSERT 時間 (s) | INSERT スループット (行/秒) | SELECT 時間 (s) | SELECT レイテンシ (ms) | DBサイズ (MB) |
| --- | ---: | ---: | ---: | ---: | ---: |
| 平文 (`10k_top10_plain.db`) | 1.193 | 8,383 | 0.0130 | 12.97 | 30.96 |
| 暗号化 (`10k_top10_encrypted.db`) | 5.774 | 1,732 | 0.123 | 123.15 | 31.60 |
- INSERT は暗号化で **約 4.8 倍** 遅延 (スループット -79%)。
- SELECT は暗号化で **約 9.5 倍** 遅延 (+110 ms)。
- DB サイズ差は約 +0.6 MB (+1.9%)。
- 取得上位 10 件の `rowid` / `hex_id` / `distance` は平文・暗号化で一致。

### Scenario B: 100,000 rows / Top 1,000
| ケース | INSERT 時間 (s) | INSERT スループット (行/秒) | SELECT 時間 (s) | SELECT レイテンシ (ms) | DBサイズ (MB) |
| --- | ---: | ---: | ---: | ---: | ---: |
| 平文 (`100k_top1000_plain.db`) | 11.98 | 8,347 | 0.153 | 152.81 | 303.59 |
| 暗号化 (`100k_top1000_encrypted.db`) | 60.12 | 1,663 | 0.650 | 650.26 | 309.49 |
- INSERT は暗号化で **約 5.0 倍** 遅延 (スループット -80%)。
- SELECT は暗号化で **約 4.3 倍** 遅延 (+497 ms)。
- DB サイズ差は約 +5.9 MB (+1.9%)。
- Top 1,000 の結果セットも平文／暗号化で同一（JSON に 1,000 行分保存）。

## 追加考察
- 規模を 10 倍に増やすと平文でも処理時間はほぼ線形に増加し、SQLCipher の暗号化オーバーヘッドは INSERT で ~5 倍、SELECT で 4〜9 倍の範囲に収束。
- SELECT の遅延差は対象行数が多いほど絶対値が大きくなり、暗号化環境で 0.5 秒級のオーバーヘッドが発生。
- ファイルサイズ増加率はデータ量によらずほぼ一定 (~1.9%)。
- さらなる検証候補:
  1. 複数試行の統計値算出と信頼区間の評価
  2. ベクトル次元／件数を変化させたスケールテスト
  3. SQLCipher PRAGMA（`cipher_page_size` など）調整による性能変化
  4. アプリケーション側の接続再利用やバッチ処理による最適化

## ファイル構成
- `scripts/generate_data.py`
- `scripts/benchmark.py`
- `data/inserts.sql`, `data/test_vector.json`
- `data/inserts_100k.sql`, `data/test_vector_100k.json`
- `data/benchmark_results.json`
- `10k_top10_plain.db`, `10k_top10_encrypted.db`
- `100k_top1000_plain.db`, `100k_top1000_encrypted.db`
- `report.md`（本書）

以上。
