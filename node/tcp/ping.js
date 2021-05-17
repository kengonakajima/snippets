var net = require("net")

var host = process.argv[2];

var cl = new net.Socket();
cl.connect( 22222, host, function() {
    cl.tot=0;
    cl.cnt=0;
    console.log("connected:",cl.remoteAddress, cl.remotePort );
    setInterval( function() {
        var b=new Uint8Array(8);
        var dv=new DataView(b.buffer);
        var t=new Date().getTime();
        dv.setFloat64(0,t,true);
        cl.write(b);
        cl.sent_at=t;
    }, 100 );
    cl.on("data", function(data) {
        var ofs=0;
        while(ofs<data.length) {
            var dv=new DataView(data.buffer);
            var nowt=new Date().getTime();
            var datat=dv.getFloat64(ofs,true);
            var dt=nowt-datat;
            cl.tot+=dt;
            cl.cnt++;
            console.log("Time:",nowt, "Cnt:",cl.cnt, " RTT:",dt,"ms", "avg:",cl.tot/cl.cnt);
            ofs+=8;
        }        
    });
    cl.on("close", function() {
        console.log("closed:", cl.remoteAddress, cl.remotePort );
    });
});


