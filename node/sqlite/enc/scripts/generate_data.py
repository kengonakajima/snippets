#!/usr/bin/env python3
import argparse
import random
from pathlib import Path

def generate_vector(rng, dim):
    return '[' + ','.join(f"{rng.uniform(-1.0, 1.0):.6f}" for _ in range(dim)) + ']'

def generate_hex(rng, length=32):
    alphabet = '0123456789abcdef'
    return ''.join(rng.choice(alphabet) for _ in range(length))

def main():
    parser = argparse.ArgumentParser(description="Generate SQL insert statements for vec_data table.")
    parser.add_argument('--rows', type=int, default=10000)
    parser.add_argument('--dim', type=int, default=768)
    parser.add_argument('--seed', type=int, default=42)
    parser.add_argument('--output-sql', type=Path, required=True)
    parser.add_argument('--output-vector', type=Path, required=True, help="Path to write test vector JSON string.")
    args = parser.parse_args()

    rng = random.Random(args.seed)
    args.output_sql.parent.mkdir(parents=True, exist_ok=True)
    args.output_vector.parent.mkdir(parents=True, exist_ok=True)

    with args.output_sql.open('w', encoding='ascii') as sql_file:
        sql_file.write('BEGIN;\n')
        for row_id in range(1, args.rows + 1):
            embedding = generate_vector(rng, args.dim)
            hex_value = generate_hex(rng)
            sql_file.write(
                f"INSERT INTO vec_data(rowid, embedding, hex_id) VALUES ({row_id}, '{embedding}', '{hex_value}');\n"
            )
        sql_file.write('COMMIT;\n')

    test_vector = generate_vector(rng, args.dim)
    args.output_vector.write_text(test_vector + '\n', encoding='ascii')

if __name__ == '__main__':
    main()
