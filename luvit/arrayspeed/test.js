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

