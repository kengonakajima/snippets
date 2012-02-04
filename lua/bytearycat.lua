function makeiary(n)
   local out={}
   for i=1,n do table.insert(out,i) end
   return out
end


function doit(n)
   local a = makeiary(n)
   local st = os.clock()
   s = table.concat( a, "" )
   local et = os.clock()
   print("n:",n, "t:",(et-st), "sl:",#s )
end

doit(1000)
doit(5000)
doit(10000)
doit(50000)
doit(100000)
doit(500000)