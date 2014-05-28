local syscall = require("syscall")

print("syscall:",syscall )


local s = syscall.socket( "inet", "stream")
s:setsockopt("socket","reuseaddr",true)
local sa = syscall.t.sockaddr_in(23333,"0.0.0.0")

print("socket:",s)
print("sa:",sa)

local ret = s:bind(sa)

print("bind:",ret)

ret = s:listen(128)

print("listen:",ret)

local fds = syscall.t.fdset()
print("fds:",fds)

-- うーん