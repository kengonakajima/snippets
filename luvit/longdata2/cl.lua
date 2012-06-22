-- echo cl

-- collectgarbage("stop")   This fixes the EFAULT but need collectgarbage("step") in after_write.

local net = require('net')
local io = require("io")
local string = require("string")
local timer = require("timer")
local table = require("table")

local sendcnt,recvcnt = 1,1
local unit = 160000 -- 79500 -- error occurs from about 79500 on osx when repn==1
local repn = 1
local mod = 119


local cl
cl = net.createConnection(61111, '127.0.0.1', function (err)
  if err then error(err) end
  print("Connected...")

  cl.writecnt = 0
  cl.tmpstrs = {}
  function cl:writewrap(str,cb)
    if cl.writecnt == 0 then
      cl.tmpstrs = {} -- clear, eventually invoke GCs 
    end
    cl.writecnt = cl.writecnt + 1
    table.insert( cl.tmpstrs, str )
    self:write(str,function(e)
        cl.writecnt = cl.writecnt - 1
        cb(e)          
      end)
  end

  cl:on("error", function(e)
      p(e)
      error("fatal")
    end)

  
  cl:on("data",function(data)
      for i=1,#data do
        local ch = string.byte(data,i,i)
        if ch == ( recvcnt % mod ) then
          if (recvcnt%100000)==0 then
            io.stderr:write( "o")
          end
        else
          io.stderr:write( "diff("..recvcnt..":" .. ch ..")")
          assert(false)
          
        end
        recvcnt = recvcnt + 1
      end      
    end)

  timer.setInterval( 1, function()
      
      for j=1,repn do
        local t={}
        for i=1,unit do
          table.insert( t, string.char(sendcnt%mod) )
          sendcnt = sendcnt + 1
        end
        local s = table.concat( t )

        cl:write(s,function(e)
            io.stderr:write("w")
            if e then assert(false,"we:"..e) end
          end)
      end      
    end)

end)


