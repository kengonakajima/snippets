

function poo(a,b,c) {
    console.log("a:",a);
    console.log("b:",b);
    console.log("c:",c);    
}


poo.apply(this, [1,2,3,4,5]);