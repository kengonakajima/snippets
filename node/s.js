function F() {
    this.a=1;
    this.b=2;
}

f=new F();

console.log( "hoge:", f.a );
console.log( "hoge2:", f["a"] );