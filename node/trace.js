function poo() {
var stack=new Error().stack;
    console.log(stack);


    console.trace("Z");
}

poo();