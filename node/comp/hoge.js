var snappy = require("snappy");

snappy.compress("hogehogehogehoge", function(err,compressed) {
    console.log("compressed:", compressed );

    snappy.uncompress(compressed,  {asBuffer: false}, function(err,orig) {
        console.log("orig:",orig);
    } );
})