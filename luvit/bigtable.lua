local uv = require("uv_native")

function bench(title, f)
  local st = uv.hrtime()
  f()
  local et = uv.hrtime()
  local t = (et-st)/1000000
  print( title, t, "ms" )
end

function setup(n)
  local t = {}
  for i=1,n do
    t[ "k" .. i ] = i
  end
  return t
end

local t1 = setup(1)
local t1k = setup(1000)
local t1m = setup(1000000)
  

local n = 10000
bench( "empty", function() end)

bench( "t1", function()
    for i=1,100000 do
      t1["k"..i] = i
    end    
  end)

bench( "t1k", function()
    for i=1,100000 do
      t1k["k"..i] = i
    end
  end)

bench( "t1m", function()
    for i=1,100000 do
      t1m["k"..i] = i
    end
  end)


-- luvit bigtable.lua
-- empty	0.044747	ms
-- t1	171.062404	ms
-- t1k	123.12747	ms
-- t1m	103.336793	ms        faster than smaller tables..
