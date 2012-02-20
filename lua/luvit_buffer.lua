local table = require("table")
local Buffer = require("buffer").Buffer
local string = require("string")

local b10 = Buffer:new(10)
for i=1,10 do
  b10[i] = i
end
print( b10 )

-- b10[11]=11 -- error (index out of bounds)

local bs = Buffer:new( "hello" )

print( bs.length )
for i=1,bs.length do
  print(i,bs[i],string.char(bs[i]))
end

local b2 = b10 .. bs

print( #b2, b2 )