#!/usr/bin/env python3
import json
import shutil
import subprocess
import time
from dataclasses import dataclass
from pathlib import Path
from typing import Dict, List

ROOT = Path(__file__).resolve().parent.parent
SQLCIPHER_BIN = shutil.which('sqlcipher')
VEC_EXTENSION = ROOT / 'sqlite-vec' / 'dist' / 'vec0.dylib'

PASSPHRASE = 'correct horse battery staple'


@dataclass(frozen=True)
class Scenario:
    name: str
    rows: int
    top_k: int
    insert_sql: Path
    test_vector: Path

    def validate_files(self) -> None:
        if not self.insert_sql.exists():
            raise SystemExit(f'Missing insert SQL for scenario {self.name}: {self.insert_sql}')
        if not self.test_vector.exists():
            raise SystemExit(f'Missing test vector for scenario {self.name}: {self.test_vector}')


SCENARIOS = [
    Scenario(
        name='10k_top10',
        rows=10_000,
        top_k=10,
        insert_sql=ROOT / 'data' / 'inserts.sql',
        test_vector=ROOT / 'data' / 'test_vector.json',
    ),
    Scenario(
        name='100k_top1000',
        rows=100_000,
        top_k=1_000,
        insert_sql=ROOT / 'data' / 'inserts_100k.sql',
        test_vector=ROOT / 'data' / 'test_vector_100k.json',
    ),
]


@dataclass
class BenchmarkResult:
    insert_seconds: float
    insert_throughput: float
    select_seconds: float
    select_latency_ms: float
    top_rows: List[Dict[str, float]]
    db_size_bytes: int


def build_insert_script(key: str | None, scenario: Scenario) -> str:
    statements = [f'.load {VEC_EXTENSION}']
    if key:
        statements.append(f"PRAGMA key = '{key}';")
    statements.extend(
        [
            'PRAGMA journal_mode = WAL;',
            'DROP TABLE IF EXISTS vec_data;',
            f'CREATE VIRTUAL TABLE vec_data USING vec0(embedding float[{768}], hex_id text);',
            scenario.insert_sql.read_text(encoding='ascii'),
        ]
    )
    return '\n'.join(statements)


def build_select_script(key: str | None, scenario: Scenario, test_vector: str) -> str:
    statements = [f'.load {VEC_EXTENSION}']
    if key:
        statements.append(f"PRAGMA key = '{key}';")
    statements.append(
        'SELECT rowid, hex_id, distance\n'
        'FROM vec_data\n'
        f"WHERE embedding MATCH '{test_vector}'\n"
        'ORDER BY distance\n'
        f'LIMIT {scenario.top_k};'
    )
    return '\n'.join(statements)


def run_sqlcipher(db_path: Path, script: str) -> tuple[float, str]:
    if not SQLCIPHER_BIN:
        raise SystemExit('sqlcipher binary not found in PATH')
    start = time.perf_counter()
    result = subprocess.run(
        [SQLCIPHER_BIN, str(db_path)],
        input=script,
        text=True,
        capture_output=True,
        cwd=ROOT,
        check=False,
    )
    duration = time.perf_counter() - start
    if result.returncode != 0:
        raise RuntimeError(f'SQLCipher failed: {result.stderr.strip()}\nScript:\n{script[:200]}...')
    return duration, result.stdout


def parse_top_rows(output: str) -> List[Dict[str, float]]:
    rows: List[Dict[str, float]] = []
    for line in output.splitlines():
        line = line.strip()
        if not line:
            continue
        parts = line.split('|')
        if len(parts) != 3:
            continue
        rowid, hex_id, distance = parts
        rows.append({'rowid': int(rowid), 'hex_id': hex_id, 'distance': float(distance)})
    return rows


def benchmark_case(db_path: Path, key: str | None, scenario: Scenario, test_vector: str) -> BenchmarkResult:
    if db_path.exists():
        db_path.unlink()

    insert_script = build_insert_script(key, scenario)
    insert_seconds, _ = run_sqlcipher(db_path, insert_script)

    select_script = build_select_script(key, scenario, test_vector)
    run_sqlcipher(db_path, select_script)  # warm-up
    select_seconds, select_output = run_sqlcipher(db_path, select_script)

    top_rows = parse_top_rows(select_output)
    db_size = db_path.stat().st_size

    return BenchmarkResult(
        insert_seconds=insert_seconds,
        insert_throughput=scenario.rows / insert_seconds,
        select_seconds=select_seconds,
        select_latency_ms=select_seconds * 1000.0,
        top_rows=top_rows,
        db_size_bytes=db_size,
    )


def main() -> None:
    if not VEC_EXTENSION.exists():
        raise SystemExit(f'Missing sqlite-vec extension at {VEC_EXTENSION}')

    for scenario in SCENARIOS:
        scenario.validate_files()

    results: Dict[str, Dict[str, object]] = {}
    for scenario in SCENARIOS:
        print(f'Running scenario {scenario.name}...')
        test_vector = scenario.test_vector.read_text(encoding='ascii').strip()

        scenario_results: Dict[str, object] = {
            'rows': scenario.rows,
            'top_k': scenario.top_k,
        }

        cases = {
            'plain': {'db_path': ROOT / f'{scenario.name}_plain.db', 'key': None},
            'encrypted': {'db_path': ROOT / f'{scenario.name}_encrypted.db', 'key': PASSPHRASE},
        }

        for case_name, cfg in cases.items():
            print(f'  Measuring {case_name} database...')
            case_result = benchmark_case(cfg['db_path'], cfg['key'], scenario, test_vector)
            scenario_results[case_name] = {
                'insert_seconds': case_result.insert_seconds,
                'insert_throughput': case_result.insert_throughput,
                'select_seconds': case_result.select_seconds,
                'select_latency_ms': case_result.select_latency_ms,
                'db_size_bytes': case_result.db_size_bytes,
                'top_rows': case_result.top_rows,
            }
            print(
                f"    insert={case_result.insert_seconds:.3f}s "
                f"({case_result.insert_throughput:.0f} rows/s), "
                f"select={case_result.select_seconds:.3f}s"
            )

        results[scenario.name] = scenario_results

    output_path = ROOT / 'data' / 'benchmark_results.json'
    output_path.write_text(json.dumps(results, indent=2), encoding='ascii')
    print(f'Wrote results to {output_path}')


if __name__ == '__main__':
    main()
