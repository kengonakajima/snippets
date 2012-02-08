var sys = require('sys');

function f() {
    sys.print( "args:" + arguments.length );
}


f(1,2,3);
