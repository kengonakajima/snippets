function A(x,y) {
    this.a=x;
    this.b=y;
}
ary=[];
for(var i=0;i<10000000;i++) {
    ary[i]=new A(i,i);
}
setInterval( function() {
    for(var i=0;i<10000000;i++) {
        ary[i].a++;
        ary[i].b++;
    }
    console.log(new Date());
},100);
