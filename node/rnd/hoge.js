function range(a,b) {
    var small=a,big=b;
    if(big<small) {
        var tmp = big;
        big=small;
        small=tmp;
    }
    return (small + (big-small)*Math.random());

}


for(var i=0;i<100000000;i++){
    var r =range(0,4);
    if(r==4){
        console.log("pppp:",r);
    }
}