local ffi = require("ffi")


ffi.cdef[[
    void (*signal(int sig, void (*func)(int)))(int);
]]

ffi.C.signal( 2, function(n)
    print "hello"
  end)

local net = require('net')
local server
server = net.createServer(function (client)
  client:pipe(client)
  client:pipe(process.stdout)
end):listen(8080)

print("TCP echo server listening on port 8080")
