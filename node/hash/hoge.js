function hash(i) {
    var x = ((i >> 16) ^ i) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x;
}


for(var i=0;i<1000;i++){
    console.log(i, hash(i), hash(i) & 0xff, (hash(i)>>8)&0xff, (hash(i)>>16)&0xff);
}