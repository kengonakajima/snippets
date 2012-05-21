
local net = require("net")
local os = require("os")
local string = require("string")
local table = require("table")
local native = require("uv_native")

function split(str, delim)
  if string.find(str, delim) == nil then
    return { str }
  end

  local result = {}
  local pat = "(.-)" .. delim .. "()"
  local lastPos
  for part, pos in string.gfind(str, pat) do
    table.insert(result, part)
    lastPos = pos
  end
  table.insert(result, string.sub(str, lastPos))
  return result
end

        
function sendmail( from, to, subj, msg )
  local cl
  cl = net.createConnection(25, '127.0.0.1', function (err)
      if err then error(err) end

      print("connected")

      cl.got = ""
      cl:on("data", function(data)
          cl.got = cl.got .. data
          local lines = split( cl.got, "\r\n" )
          for i,l in ipairs(lines) do
            local ary = split(l, " " )
            if ary[1] == "221" then
              print("finished")
              native.close( cl._handle ) 
            end
          end
        end)
      cl:on("error", function(e)
          print("error:",e)
        end)
        
      cl:write( "HELO localhost.localdomain\r\n" )

      cl:write( "EHLO localhost.localdomain\r\n")
      cl:write( "MAIL FROM:<" .. from .. ">\r\n") 
      cl:write( "RCPT TO:<" .. to .. ">\r\n") 
      cl:write( "DATA\r\n")                 
      cl:write( "Subject: " .. subj .. "\r\n") 
      cl:write( "From: " .. from .. "\r\n")   
      cl:write( "Content-type: text/plain; charset=iso-2022-jp\r\n")
      cl:write( "Sender: " .. from .. "\r\n" )
      cl:write( "Date: " .. os.date() .. "\r\n" )
      cl:write( "To: " .. to .. "\r\n" )
      cl:write( "\r\n" )
      cl:write( "\r\n" )
      cl:write( msg )
      cl:write( "\r\n" )      
      cl:write( ".\r\n" )
      cl:write( "QUIT\r\n" )
    end)
end

sendmail( "bot@localhost.localdomain", "kengo.nakajima@gmail.com", "hogesubject", "adsasdf\r\naskdjfasdf\r\nasdkfkalsdkflaskdjflaksjdflkasjdf\r\n" )