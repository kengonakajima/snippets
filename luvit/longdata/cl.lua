local net = require('net')
local io = require("io")
local string = require("string")

local cl
cl = net.createConnection(61111, '127.0.0.1', function (err)
  if err then error(err) end
  print("Connected...")
  cl:write("hi\n")

  cl.cnt = 1
  
  cl:on("data",function(data)
      assert(data)
      assert(#data > 0 )
      for i=1,#data do
        local ch = string.byte(data,i,i)
        if ch == ( cl.cnt % 32 ) then
          if (cl.cnt%100000)==0 then
            io.stderr:write( "o")
          end
        else
          if ch == string.byte("r",1,1) then
            cl.cnt = 0
            io.stderr:write( "RST")
          elseif ch == string.byte("q",1,1) then
            print("test done. cnt:", cl.cnt)
            process.exit(0)
          else
            io.stderr:write( "e("..cl.cnt..":" .. ch ..")")
          end          
        end

        
        cl.cnt = cl.cnt + 1
      end      
    end)

end)

