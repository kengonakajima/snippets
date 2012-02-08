-- luv : libuv binding for lua

--collectgarbage( "stop" )

require "luv"


local loop = luv_default_loop()

local tcp = luv_tcp_init(loop)
print("tcp_init: tcp:",tcp)

local r = luv_tcp_bind( tcp, "0.0.0.0", 22222 )
print("tcp_bind: r:",r)

function echo_after_close(handle,status)
   print("echo_after_close:", handle, status)
end

function echo_after_shutdown(handle,status)
   print("echo_after_shutdown: ", handle, status)
   luv_close(handle, echo_after_close)
end

function echo_after_read(handle,s,nread)
--   print("echo_after_read-lua: ", handle, nread, bufbase, buflen )   
   if nread < 0 then
      print("echo_after_read-lua: error, to shutdown!" )
      luv_shutdown(handle, echo_after_shutdown )
   elseif nread == 0 then
      print("no data, ignore")
   else
      if not readCnt then readCnt = 1 else readCnt = readCnt + 1 end
      if (readCnt%10000)==0 then
         print("data from client!:",readCnt)
      end      
      luv_write( handle,s, nread )
   end
end

function on_newconnection(sv,status)
   print( "on_newconnection-lua: callback! sv,id,status:", sv,status )   
   assert(status==0)

   local stream = luv_tcp_init(loop)
   assert(stream)
   local r = luv_accept(sv, stream)
   assert(r==0)
  r = luv_read_start(stream, echo_after_read )
end


r = luv_tcp_listen( tcp, 100, on_newconnection )

-- clients

readCnt=0
function cli_after_read(sock,s,nread)
   readCnt = readCnt + 1
   if (readCnt%10000)==0 then
      print("cli_after_read:", readCnt, s )
   end   
   local r = luv_write(sock,s,nread)
   assert(r==0)
end

function cli_after_connect(sock,status)
   print("cli_after_connect: sock:", sock.id,  status )
   local r = luv_read_start(sock, cli_after_read )
   local s = "hello world"
   r = luv_write( sock, s, string.len(s) )
   assert(r==0)   
end

local clients={}
for i=1,10 do
   local cl = luv_tcp_init(loop)
   r = luv_tcp_connect( cl, "127.0.0.1", 22222, cli_after_connect )
   assert(r==0)
   table.insert( clients, cl )
end


timerCnt=0
function on_timer(timer,status)
   print("on_timer:", timer, status)
   timerCnt = timerCnt + 1
   print("timerCnt:", timerCnt )
--      luv_timer_stop(timer)
end

local t = luv_timer_init(loop)
luv_timer_start(t, on_timer, 500, 500)


print("start event loop")
luv_run( loop )

