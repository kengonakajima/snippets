local ffi=require("ffi")

ffi.cdef[[
      int qqq();
      int random();
]]

n=100000000
tot=0
for i=1,n do
   tot = tot + ffi.C.qqq()
end

print(tot)
