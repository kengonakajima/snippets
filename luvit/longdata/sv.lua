local net = require('net')
local io = require("io")
local string = require("string")
local table = require("table")

function mkstr(n)
  local t = {}
  for i=1,n do
    table.insert(t, string.char(i%119) )
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

    cl.sent = false
    cl.cnt = 1
    cl:on("data", function(data)
        assert(data)
        if #data == 0 then return end
        
        if data ~= "hi\n" then
          for i=1,#data do
            local ch = string.byte(data,i,i)
            if ch == (cl.cnt%119) then
              if( cl.cnt%200000)==0 then
                io.stderr:write("o")
              end
            else
              if ch == string.byte("r",1,1) then
                cl.cnt = 0
                io.stderr:write( "R")
              elseif ch == string.byte("q",1,1) then
                print("DONE")
              else
                io.stderr:write( "e("..cl.cnt..":" .. i .. ":" .. ch ..")")
                if i > 1 then
                  io.stderr:write( "before:".. string.byte(data,i-1,i-1) .. " ")
                end                
                assert(false)
              end
            end
            cl.cnt = cl.cnt + 1
          end
          io.stderr:write( "" .. string.byte(data,#data,#data) .. " ")
        end
        

        if not cl.sent then
          cl.sent = true
          for i=1,1000 do
            cl:write(s1000000)
            cl:write("r")
          end
          cl:write("q")
        end        
      end)
    
    
  end):listen(61111)

print("start")
