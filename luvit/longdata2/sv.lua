-- simple echo sv
local net = require('net')
local io = require("io")
local string = require("string")
local table = require("table")

local cnt = 1
local unit = 119

local server
server = net.createServer(function (cl)
    cl:on("error", function(e)
         io.stderr:write("E") 
      end)

    cl:on("end", function() cnt=1 end)    

    cl:on("data", function(data)
        cl:write(data)
        io.stderr:write(".")
        for i=1,#data do
          local ch = string.byte(data,i,i)
--          io.stderr:write( string.format("%02x ",ch) )
          if (cnt%unit)~=ch then
            io.stderr:write("match error at:",cnt)
          end          
          cnt = cnt + 1
        end
      end)
  end):listen(61111)

print("start")
