// polygon clipping
const polygonClipping = require('polygon-clipping')

const poly1 = [[[0,0],[2,0],[0,2],[0,0]]]
const poly2 = [[[-1,0],[1,0],[0,1],[-1,0]]]

const union=polygonClipping.union       (poly1, poly2 /* , poly3, ... */)
polygonClipping.intersection(poly1, poly2 /* , poly3, ... */)
polygonClipping.xor         (poly1, poly2 /* , poly3, ... */)
const dif = polygonClipping.difference  (poly1, poly2 /* , poly3, ... */)

//console.log(JSON.stringify(union));


function makeCircle(lat,lng,radius,n)
{
    var ary=[];
    var angle=Math.PI*2.0/n;
    for(let i=0;i<n;i++) {
        let x=Math.cos(i*angle)*radius;
        let y=Math.sin(i*angle)*radius;
        ary.push( [lng+x, lat+y] );
    }
    return ary;
}


const c0 = makeCircle(137,37,0.6,32);
const c1 = makeCircle(136,37,0.6,32);
const c2 = makeCircle(135,37,0.2,32);
const u = polygonClipping.union([[c0],[c1],[c2]]);

console.log(JSON.stringify(u));

console.log("L:",u.length, u[0].length, u[0][0].length);