function f() {
    echo "poo $1"
}

HOGE="$(f $1) $(f)"

echo $HOGE



