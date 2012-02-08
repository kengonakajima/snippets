var Proxy = require("./node-proxy");

var sys = require("sys");

var p = Proxy.create({
    get: function(proxy,name) {
        return 'Hello, ' + name ;
    }

});

sys.print( p.World );
