
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