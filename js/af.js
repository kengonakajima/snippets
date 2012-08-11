//

function g(x) {
    return x + 1;
}

var f = function(x) {
    return x + 1;
}

//var funcs = [ f,f,f,f,f,f,f,f,f,f ];
var funcs =   [ g,g,g,g,g,g,g,g,g,g ];

var cnt=0;
function bench() {
    cnt++;
    console.log("start:",cnt);    
    for(var i=0;i<10000000;i++){
        var f = funcs[i%10];
        g(i);
    }
    console.log("end");
}


setInterval( bench, 1000 );
