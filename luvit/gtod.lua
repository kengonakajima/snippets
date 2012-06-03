ffi=require("ffi")

ffi.cdef[[
    struct timeval {
      long  tv_sec;   
      long  tv_usec;  
    };    
    int gettimeofday(struct timeval *restrict tp, void *restrict tzp);
]]

tmv = ffi.new( "struct timeval")
ffi.C.gettimeofday( tmv, nil )
print(tonumber(tmv.tv_sec))
print(tonumber(tmv.tv_usec))