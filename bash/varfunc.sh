function f() {
    echo "poo $1"
}

HOGE="$(f aa) $(f)"

echo $HOGE



