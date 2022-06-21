var polygonBoolean = require('2d-polygon-boolean');

var p0 = [36.70011728595625, 137.2212436497083];
var p1 = [36.70024363700027, 137.22274390064425];


function makeCircle(pos,radius,n)
{
    var ary=[];
    var angle=Math.PI*2.0/n;
    for(let i=0;i<n;i++) {
        let x=Math.cos(i*angle)*radius;
        let y=Math.sin(i*angle)*radius;
        ary.push( [pos[0]+x, pos[1]+y] );
    }
    return ary;
}

var r=0.001;
var c0 = makeCircle(p0,r,32);
var c1 = makeCircle(p1,r,32);

console.log("c0:",c0);
console.log("c1:",c1);

var union = polygonBoolean(c0, c1, "or");

console.log("union:",union);

/*
var subject = [
  [0, 0],
  [100, 0],
  [100, 100],
  [0, 100]
];

var clip = [
  [90, 90],
  [110, 90],
  [110, 110],
  [90, 110],
  [90, 90]
];


var union = polygonBoolean(subject, clip, 'or');
console.log('union results', union);

var cut = polygonBoolean(subject, clip, 'not');
console.log('cut results', cut);

var intersect = polygonBoolean(subject, clip, 'and');
console.log('intersect results', intersect);
*/