
function hoge() {
    console.log(arguments);
}


hoge(1);

hoge("a",22);

function poo() {
    arguments = 2;
    console.log(arguments);
}

poo("zzzae");

function fuga() {
    console.log(typeof arguments);
// not an array    console.log(arguments.slice(2));
}

fuga([1,2,3]);
