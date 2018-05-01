

////

var moyai_rng_w = 123456789;
var moyai_rng_z = 987654321;
var moyai_rng_mask = 0xffffffff;

// Takes any integer
function moyai_rng_seed(i) {
    moyai_rng_w = i;
    moyai_rng_z = 987654321;
}

// Returns number between 0 (inclusive) and 1.0 (exclusive),
// just like Math.random().
function moyai_rng_random()
{
    moyai_rng_z = (36969 * (moyai_rng_z & 65535) + (moyai_rng_z >> 16)) & moyai_rng_mask;
    moyai_rng_w = (18000 * (moyai_rng_w & 65535) + (moyai_rng_w >> 16)) & moyai_rng_mask;
    var result = ((moyai_rng_z << 16) + moyai_rng_w) & moyai_rng_mask;
    result /= 4294967296;
    return result + 0.5;
}

irange = function(a,b) {
    return Math.floor(range(a,b));
}
range = function(a,b) {
    var small=a,big=b;
    if(big<small) {
        var tmp = big;
        big=small;
        small=tmp;
    }
    var out=(small + (big-small)*moyai_rng_random());
    if(out==b)return a; // in very rare case, out==b
    return out;
}

hrnow = function() {
    var t=process.hrtime();
    return t[0] + t[1]/1000000000.0;
}


var n=16*16*16;
var simple=[];
var bulk={bt:new Array(n), lt:new Array(n), hoge:new Array(n), fuga:new Array(n), tt:new Array(n), st:new Array(n) };

var t0=hrnow();
for(var i=0;i<n;i++) {
    var c={bt:irange(0,3),lt:irange(0,15), hoge:irange(0,2), fuga:irange(0,10), tt:i%4, st:0 };
    simple.push(c);
    bulk.bt[i]=c.bt;
    bulk.lt[i]=c.lt;
    bulk.hoge[i]=c.hoge;
    bulk.fuga[i]=c.fuga;
    bulk.tt[i]=c.tt;
    bulk.st[i]=c.st;
}
var t1=hrnow();
var simple_s=JSON.stringify(simple);
var t2=hrnow();

console.log("simple:",simple_s.length, t2-t1);


var t3=hrnow();
var bulk_s=JSON.stringify(bulk);
var t4=hrnow();
console.log("bulk:", bulk_s.length, t4-t3);


var snappy=require("snappy");

var t5=hrnow();
snappy.compress(simple_s, function(err,compd) {
    var t6=hrnow();
    console.log("snappyed_simple:", compd.length, t6-t5);
});


var t7=hrnow();
snappy.compress(bulk_s, function(err,compd) {
    var t8=hrnow();
    console.log("snappyed_bulk:", compd.length, t8-t7);
});
