async function tako(x) {
    console.log("tako");
    return x*2;
}
async function poo(x) {
    console.log("poo");
    return x+await tako(x);
}

var x=poo(8);

console.log("x:",x);

setTimeout( () => {
    console.log("xxxxx:",x) ;
});

