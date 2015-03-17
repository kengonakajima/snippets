var a = 130;

if(a>=128) {
    var b = (a - 128) * -1;
    var c = b << 8;
    console.log( "c:", c );    
    var d = c >> 8;
    console.log( "d:", d );
    console.log((d<<8) & 0xff);
    console.log((d<<16) & 0xff);    
}