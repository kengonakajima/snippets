var a=function(cb,x) {
    for(var i=0;i<10000000;i++) {
        x+=1;
    }
    cb(100+x);
}

var f=function(i) {
    console.log("f:i:",i);
}

console.log("sss");
a( f,20 );
console.log("eee");