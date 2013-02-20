void hoge( int arg[static 10] ) {
    
}

int main(){
    int a[10];
    hoge(a);
    int *b;
    hoge(b);
    int c[5];
    hoge(c);
    hoge(0);
    return 0;
}
