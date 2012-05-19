local ffi = require("ffi")


ffi.cdef[[
    int getpid();
]]

print( ffi.C.getpid() )
