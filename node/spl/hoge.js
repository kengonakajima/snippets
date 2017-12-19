
function splitArray(ary,unitsize) {
    var out=[];
    for(var i=0;;i++) {
        var start_ind=i*unitsize;
        var end_ind=(i+1)*unitsize;
        var to_break=false;
        console.log("xxx:",start_ind, end_ind);
        if( end_ind >= ary.length ){
            end_ind=ary.length;
            to_break=true;
        }
        out.push( ary.slice(start_ind, end_ind) );
        if( to_break ) break;
    }
    return out;
}


a=[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18];
aa=splitArray(a,4);

console.log(aa);