#!/bin/bash

# 出力先のテキストファイル名
output_file="inhabited_times.txt"

# 既存の出力ファイルがあれば削除
rm -f "$output_file"

# 現在のディレクトリ内のすべての.mcaファイルを処理
for file in *.mca; do
    echo "Processing $file..."

    # 指定されたコマンドを実行し、最大のInhabitedTimeを取得
    max_inhabited_time=$(node parse.js "$file" | grep Inhabi | sed -E 's/.*"InhabitedTime": "([0-9]+)".*/\1/' | sort -n | tail -1)

    # InhabitedTimeが取得できなかった場合の処理
    if [ -z "$max_inhabited_time" ]; then
        echo "$file N/A" >> "$output_file"
    else
        echo "$file $max_inhabited_time" >> "$output_file"
    fi
done

echo "結果が $output_file に保存されました。"

