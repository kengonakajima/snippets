-- luv : libuv binding for lua

local ffi = require "ffi"

ffi.cdef[[
      
      void *luv_default_loop();
      void luv_run(void*);

      void* luv_tcp_init( void*loop);
      int luv_tcp_bind( void*svh, void *addrb, int port );
      
      typedef void ( __stdcall *afterlistenproc)( void*sv, int st);
      int luv_tcp_listen( void *tcp, int maxcon, afterlistenproc cb );

      int luv_accept( void*server, void *stream );
      
      typedef void ( __stdcall *afterreadproc)(void *stream, int id, int nread, char *bufbase, int buflen );
      void luv_set_after_read_callback_wrapper( afterreadproc cb );
      int luv_read_start( void *stream );

      typedef void (*afterwriteproc)(void*handle,int);      
      int luv_write( void *handle, void *tosend, int buflen);
      void luv_set_after_write_callback_wrapper( afterwriteproc cb);

      typedef void (*aftershutdownproc)(void*handle,int);
      int luv_shutdown( void *handle, aftershutdownproc cb );

      typedef void (*aftercloseproc)(void*handle);
      void luv_close( void *handle, aftercloseproc cb );

      typedef void (*afterconnectproc)(void*handle,int id, int status);
      int luv_tcp_connect( void *tcp, void *addrb, int port, afterconnectproc cb );

      typedef void (*timerproc) (void*handle, int status);
      void* luv_timer_init(void*loop);
      int luv_timer_start(void*timer, timerproc cb, double timeout, double rept );
      int luv_timer_stop(void*timer);                          

      int luv_tcp_id(void*handle);

]]

function cstr(s)
   local b = ffi.new("char[?]", string.len(s)+1)
   ffi.copy(b,s)
   return b
end

---------------------

local socks={}


local luv = ffi.load("luvwrap")




luv_default_loop = luv.luv_default_loop
function luv_tcp_init(loop)
   local tcp = luv.luv_tcp_init(loop)
   if not tcp then return nil end
   local newid = luv.luv_tcp_id(tcp) 
   local newsock = { handle=tcp, id=newid }
   socks[newid] = newsock
   return newsock
end

function luv_tcp_bind(sock,a,p)
   return luv.luv_tcp_bind( sock.handle, cstr(a), p )
end

function _luv_newconnection_cb_wrap(sv,status)
   local svid = luv.luv_tcp_id(sv)
   assert(svid>0)
   local sock = socks[svid]
   return sock.newconnection_cb(sock,status)
end

function luv_tcp_listen(sock,maxcon,cb)
   sock.newconnection_cb=cb
   return luv.luv_tcp_listen(sock.handle,maxcon,_luv_newconnection_cb_wrap)
end

function _luv_afterwrite_cb_wrap(handle,status)
   local sockid = luv.luv_tcp_id(handle)
   assert(sockid>0)
   local sock = socks[sockid]
   return sock.afterwrite_cb(sock,status)
end

luv.luv_set_after_write_callback_wrapper( _luv_afterwrite_cb_wrap )

function luv_write(sock,str,l,cb)
   sock.afterwrite_cb=cb
   return luv.luv_write(sock.handle,cstr(str),l)
end

function _luv_afterconnect_cb_wrap(tcp,id,status)
   print( "_luv_afterconnect_cb_wrap:",tcp,id,status)
   local sock = socks[id]
   return sock.afterconnect_cb(sock,status)
end

function luv_tcp_connect(sock,a,p,cb)
   sock.afterconnect_cb=cb
   return luv.luv_tcp_connect(sock.handle,cstr(a),p,_luv_afterconnect_cb_wrap)
end



function _luv_afterread_cb_wrap(tcp,id,nread,bufbase,buflen)
   local sock=socks[id]
   if nread <= 0 then
      return sock.afterread_cb(sock,nil,nread)
   else
      return sock.afterread_cb(sock,ffi.string(bufbase,nread),nread)
   end   
end

luv.luv_set_after_read_callback_wrapper( _luv_afterread_cb_wrap )

function luv_read_start(sock,cb)
   sock.afterread_cb=cb
   return luv.luv_read_start(sock.handle)
end

function luv_accept(sv,sock)
   return luv.luv_accept(sv.handle,sock.handle)
end


function luv_close(sock,cb)
   return luv.luv_close(sock.handle,cb)
end

function luv_shutdown(sock,cb)
   return luv.luv_shutdown(sock.handle,cb)
end

luv_timer_init = luv.luv_timer_init
luv_timer_stop = luv.luv_timer_stop
luv_timer_start = luv.luv_timer_start
luv_run = luv.luv_run

luv_tcp_id=luv.luv_tcp_id



function string:split(sSeparator, nMax, bRegexp)
	assert(sSeparator ~= '')
	assert(nMax == nil or nMax >= 1)

	local aRecord = {}

	if self:len() > 0 then
		local bPlain = not bRegexp
		nMax = nMax or -1

		local nField=1 nStart=1
		local nFirst,nLast = self:find(sSeparator, nStart, bPlain)
		while nFirst and nMax ~= 0 do
			aRecord[nField] = self:sub(nStart, nFirst-1)
			nField = nField+1
			nStart = nLast+1
			nFirst,nLast = self:find(sSeparator, nStart, bPlain)
			nMax = nMax-1
		end
		aRecord[nField] = self:sub(nStart)
	end

	return aRecord
end