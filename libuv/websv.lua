require "luv"

local loop = luv_default_loop()
local sv = luv_tcp_init(loop)
local r = luv_tcp_bind( sv, "0.0.0.0", 22280 )


local response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 13\r\n\r\nhello world\r\n"

httpCnt=0

function after_write(sock,status)
   luv_close(sock)
end


function on_read(sock,str,nread)
--   print("on_read:id:",sock.id,str,nread, " state:", sock.state)
   if nread <= 0 then
      print("end")
      return
   end
   
   if not sock.buf then
      sock.buf = str
   else
      sock.buf = sock.buf .. str
   end
   local ary = sock.buf:split("\r\n")   
   if ary[#ary] == "" and ary[#ary-1] == "" and
      ( ary[1] == "GET / HTTP/1.1" or ary[1] == "GET / HTTP/1.0" ) then
      httpCnt = httpCnt + 1
      if (httpCnt%100)==0 then print(httpCnt) end      
      luv_write(sock, response, string.len(response), after_write )      
   end

end
function on_newconnection(sv,status)
   local newsock = luv_tcp_init(loop)
--   print("new con. status:",status, " newsock:", newsock, " newid:", newsock.id )
   r=luv_accept(sv,newsock)
   assert(r==0)
   newsock.state = "hello-state"
   luv_read_start(newsock,on_read)
end

r = luv_tcp_listen( sv, 100, on_newconnection )
assert(r==0)

print("start loop")
luv_run(loop)
