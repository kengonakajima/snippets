function makeiary(n)
   local out={}
   for i=1,n do table.insert(out,i*1.1e-33) end
   return out
end


function doit(n)
   local a = makeiary(n)
   local st = os.clock()
   local s = "return {" .. table.concat(a, "," ) .. "}"
   local f = loadstring(s)
   local t = f()
   local et = os.clock()
   print("n:",n, "t:",(et-st), "sl:",#s, "t:",t[1] )
   return s
end

doit(1000)
doit(10000)
doit(20000)
doit(30000)
doit(50000)
doit(100000)