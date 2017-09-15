var ping = require("net-ping")



var target = "8.8.8.8";

setInterval( function() {
    var session = ping.createSession();
    session.pingHost( target, function(error,target) {
        if(error) {
            console.log("target:" + target + " : " + error.toString() );
        } else {
            console.log("target:" + target + " is alive");
        }
    });    
}, 500 );
