function f() {
    echo poo
}

HOGE="$(f) $(f)"

echo $HOGE



