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
  print( json.stringify({ time=(b-a)/1000/1000, title=title } ))
end

benchmark( function() a = v                 end, 'direct' )
benchmark( function() a = a1[1]             end, 'array depth-1')
benchmark( function() a = a2[1][1]          end, 'array depth-2')
benchmark( function() a = a3[1][1][1]       end, 'array depth-3')
benchmark( function() a = a4[1][1][1][1]    end, 'array depth-4')
benchmark( function() a = a5[1][1][1][1][1] end, 'array depth-5')


