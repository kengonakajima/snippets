local net = require('net')
local io = require("io")
local string = require("string")
local table = require("table")

function mkstr(n)
  local t = {}
  for i=1,n do
    table.insert(t, string.char(i%32) )
  end
  return table.concat( t,"")
end

local s1000 = mkstr(1000)
local s10000 = mkstr(10000)
local s100000 = mkstr(100000)
local s1000000 = mkstr(1000000)

local server
server = net.createServer(function (cl)
    cl:on("error", function(e)
         io.stderr:write("E") 
      end)

    cl:on("data", function(data)
        print("data:",#data)
        io.stderr:write(".")

        for i=1,1000 do
          cl:write(s1000000)
          cl:write("r")
        end

        cl:write("q")
      end)
    
    
  end):listen(61111)

print("start")
