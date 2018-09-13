var aho : number = 1;

var l : { [hoge:number]:number } = {};
l[10]=100;
console.log(l);

for(var q in l) {
    console.log(q.hoge);
}
