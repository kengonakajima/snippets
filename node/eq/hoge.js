var cnt=0;
var st=new Date().getTime();
for(var j=0;j<100000;j++) {
    for(var i=0;i<100000;i++) {
        if(i===j)cnt++;
    }
}
var et=new Date().getTime();
console.log("cnt:",cnt,et-st);