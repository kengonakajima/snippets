var qrcode=require("qrcode");

qrcode.toDataURL( "hello world", function(err,url) {
    console.log(err,url);
})
