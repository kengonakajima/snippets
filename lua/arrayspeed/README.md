JavaScript code
----

var v = 'Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.';

var
  a1 =     [v],
  a2 =    [[v]],
  a3 =   [[[v]]],
  a4 =  [[[[v]]]],
  a5 = [[[[[v]]]]];

var benchmark = function(f, title){
  var a = new Date();
  for(var n = 100*1000*1000; n; --n)
    f();
  var b = new Date();
  console.log( { time:b-a, title:title, unit:"ms" } );
};

benchmark( function(){ var a = v;                 }, 'direct' );
benchmark( function(){ var a = a1[0];             }, 'array depth-1');
benchmark( function(){ var a = a2[0][0];          }, 'array depth-2');
benchmark( function(){ var a = a3[0][0][0];       }, 'array depth-3');
benchmark( function(){ var a = a4[0][0][0][0];    }, 'array depth-4');
benchmark( function(){ var a = a5[0][0][0][0][0]; }, 'array depth-5');

Lua code
----

local uv = require("uv_native")
local json = require("json")

v = "Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum."

a1 =     {v}
a2 =    {{v}}
a3 =   {{{v}}}
a4 =  {{{{v}}}}
a5 = {{{{{v}}}}}

function benchmark(f, title)
  local a = uv.hrtime()
  for n = 1,100*1000*1000 do
    f()
  end
  local b = uv.hrtime()
  print( json.stringify({ time=(b-a)/1000/1000, title=title, unit="ms" } ))
end

benchmark( function() a = v                 end, 'direct' )
benchmark( function() a = a1[1]             end, 'array depth-1')
benchmark( function() a = a2[1][1]          end, 'array depth-2')
benchmark( function() a = a3[1][1][1]       end, 'array depth-3')
benchmark( function() a = a4[1][1][1][1]    end, 'array depth-4')
benchmark( function() a = a5[1][1][1][1][1] end, 'array depth-5')


Result
----

make -k 
node --version
v0.6.14
node test.js
{ time: 909, title: 'direct', unit: 'ms' }
{ time: 1102, title: 'array depth-1', unit: 'ms' }
{ time: 1266, title: 'array depth-2', unit: 'ms' }
{ time: 1631, title: 'array depth-3', unit: 'ms' }
{ time: 2012, title: 'array depth-4', unit: 'ms' }
{ time: 2333, title: 'array depth-5', unit: 'ms' }
luvit --version
0.3.0-8-g3da0b2d
luvit test.lua
{"time":70.317002,"unit":"ms","title":"direct"}
{"time":72.317853,"unit":"ms","title":"array depth-1"}
{"time":67.996615,"unit":"ms","title":"array depth-2"}
{"time":70.527224,"unit":"ms","title":"array depth-3"}
{"time":80.763861,"unit":"ms","title":"array depth-4"}
{"time":94.745047,"unit":"ms","title":"array depth-5"}

Conclusion
----
luvit is about 15-20 times faster than node.