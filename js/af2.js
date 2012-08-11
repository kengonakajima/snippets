//

function g(x) {
    return x + 1;
}

var cnt=0;
function bench() {
    cnt++;
    console.log("start:",cnt);    
    for(var i=0;i<10000000;i++){
        g(i);
    }
    console.log("end");
}


setInterval( g, 1 );
setInterval( bench, 1000 );
