-- speed test of string.len

-- slow!
function makestr(n)
   local out=""
   for i=1,n-1 do out = out .. "a" end
   out = out .. "b"
   return out
end

function doit(n)
   local s = makestr(n)
   local st = os.clock()
   for i=1,100000 do
      local sl = string.len(s)
   end
   local et = os.clock()
   print("n:", n, "t:",(et-st))
end


doit(10)
doit(100)
doit(1000)
doit(10000)
doit(100000)
