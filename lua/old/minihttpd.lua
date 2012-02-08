local tcp = require('tcp')
local timer = require("timer")
local coroutine = require("coroutine")
local table = require("table")
local io = require("io")
local string = require("string")

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

tasks = {}
function startStream(client, data )
   local task = {
      client = client,
      data = data,
      sentlen = 0,
      func = function(self)
                local tosend = #self.data - self.sentlen
                if tosend > 0 then
                   local unit = 8192
                   if tosend > unit then tosend = unit end
                   print("sending len:", tosend, "sent:", self.sentlen)
                   self.client:write( string.sub( self.data, self.sentlen+1, self.sentlen + tosend) )
                   self.sentlen = self.sentlen + tosend
                   return true
                else
                   print("stream finish,close.")
                   self.client:close()
                   return false
                end
             end
   }
   table.insert(tasks, task )
   return task
end


tcp.create_server(
   "0.0.0.0", 8180,
   function(client)
      client.recvbuf = ""
      client:on( "data", function(chunk)
                            print( "data from sock: chunk:", #chunk )
                            client.recvbuf = client.recvbuf .. chunk

                            local lines = split( client.recvbuf, "\r\n" )
                            print("numline:", #lines )
                            for i,v in ipairs(lines) do
                               print( "line:",i,v )
                            end
                            if lines[ #lines] == "" and lines[#lines-1] == "" then
                               print("HEADER FINISHED. first line:", lines[1] )
                               local tks = split(lines[1], " ")
                               if #tks == 3 and tks[1] == "GET" then
                                  local path = "./" .. tks[2]
                                  if string.sub( path, #path, #path ) == "/" then
                                     client:write( "HTTP/1.0 403 Forbidden\r\n\r\n" )
                                     client:close()
                                  else
                                     print( "url:", tks[2] )
                                     local fp = io.open(path,"rb")
                                     if fp then
                                        local data = fp:read(1024*1024*16)
                                        fp:close()
                                        
                                        if not data then data = "" end
                                        client:write( "HTTP/1.0 200 OK\r\nContent-Length: " .. #data .. "\r\n\r\n" )
                                        if #data > 0 then
                                           startStream( client, data )
                                        else
                                           print("closing")
                                           client:close()
                                        end
                                     else
                                        client:write( "HTTP/1.0 404 Not Found\r\nContent-Length: 0\r\n\r\n" )
                                     end
                                  end
                               end
                            end
                         end)
   end)


timer.set_interval( 100,
                    function()
                       print("interval. tasks:", #tasks )
                       for i,task in ipairs(tasks) do
                          if task:func() == false then
                             table.remove(tasks, i)
                             break
                          end
                       end
                    end)
                             

print("Http static file server listening")
