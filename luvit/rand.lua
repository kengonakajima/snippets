require( "./lumino/lumino")
math = require( "math") 
local n = 1000*1000*100

t =measure( function()
    tot=0
    for i=1,n do
      tot = tot + math.random()
    end
    print(tot)
  end)


print(t)
