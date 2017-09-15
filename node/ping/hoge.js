var ping = require("net-ping")

var session = ping.createSession();

var target = "8.8.8.8";

session.pingHost( target, function(error,target) {
    if(error) {
        console.log("target:" + target + " : " + error.toString() );
    } else {
        console.log("target:" + target + " is alive");
    }
});