var ary = new Uint8Array(8);
ary[0] = 0;
ary[1] = 0;
ary[2] = 0;
ary[3] = 4;
ary[4] = 0;
ary[5] = 0;
ary[6] = 0;
ary[7] = 10;

function get32nb(ui8ary,sgn,ofs) {
    var sa = ui8ary.subarray(ofs,4);
    var ui32 = (sa[0] << 24) + (sa[1]<<16) + (sa[2]<<8) + sa[3];
    if(sgn) {
        return Math.imul(ui32,1); // (0xffffffff,1) -> -1
    } else {
        return ui32;
    }    
}