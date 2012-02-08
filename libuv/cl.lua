require "luv"


local loop = luv_default_loop()
local sock = luv_tcp_init(loop)
local r = luv_tcp_bind( sock, "0.0.0.0", 22222 )

cnt=0

function on_data(sock,id,s,nread)
   if (cnt%1000)==0 then
      print("cnt:",cnt, s,id)
   end   
   cnt = cnt + 1
   local s = "hello" .. cnt
   luv_write( sock, s, string.len(s))
end

function on_connect(sock,status)
   print("on_connect: id,st:",sock.id,status)
   luv_read_start(sock, on_data )
   luv_write(sock, "hello",5)
end
print("start connecting.. id:", sock.id)
r = luv_tcp_connect(sock,"127.0.0.1", 22222, on_connect )

luv_run(loop)                    