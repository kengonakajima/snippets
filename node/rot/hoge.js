// 0,1, 2,2,3,3, 0,0,0,1,1,1, 2,2,2,2,3,3,3,3,0,0,0,0,0,1,1,1,1,1,2,2,2,2,2,2,3,3,3,3,3,3,0,0,0,0,0,0,0,..
function hoge(n) {
    var step=0;
    var unit=1;
    var dir=0;
    var ary=[];
    for(var i=0;i<n;i++) {
        ary.push(dir%4);
        step++;
        if(step==unit) {
            dir++;step=0;
            if(dir==2||dir==4) { unit++; }            
        }
    }
    console.log(ary);
}

hoge(20);    
