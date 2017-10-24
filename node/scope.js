var p=3;

function hoge() {
    var q=3;
    p++;
    console.log("in:",p);
    function poo() {
        p++;
        q++;
        console.log("in poo:",p,q);        
    }
    poo();
}

hoge();
console.log("out:",p);