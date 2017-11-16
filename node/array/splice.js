a=[55,66,77,11,4,5,778,9];

for(var i=0;i<a.length;i++) {
    if(a[i]==11||a[i]==5) {
        a.splice(i,1);
    }
}

console.log(a);

        
    