var p1=new Promise( function(res,rej) {
    console.log("p1");
    res(111);
});

var p2=p1.then(function(val) {
    console.log("p2",val) ;
});