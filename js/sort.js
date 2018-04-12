a=[ {x:1, y:3}, {x:10, y:8}, {x:4, y:4 }];
a.sort(function(a,b) {
    return (a.x>b.x);
});
console.log(a);