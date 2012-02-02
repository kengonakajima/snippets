-- speed test of string.substr

-- slow!
function makestr(n)
   local out=""
   for i=1,n-1 do out = out .. "a" end
   out = out .. "b"
   return out
end

a = makestr(200000)

function doit(n,l)
   local st = os.clock()
   for i=1,100000 do
      local s = string.sub( a, n/2,n/2+l)
      if s == "a" then
      end   
   end
   local et = os.clock()
   print("n:", n, "l:",l, "t:",(et-st))
end


doit(100,10)
doit(1000,10)
doit(10000,10)
doit(10000,100)
doit(10000,1000)
doit(100000,10)
doit(100000,100)
doit(100000,1000)
doit(100000,10000)
doit(200000,10)
doit(200000,100)
doit(200000,1000)
doit(200000,10000)
