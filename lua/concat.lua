function makeiary(n)
   local out={}
   for i=1,n do table.insert(out,i) end
   return out
end


function doit(n)
   local a = makeiary(n)
   local b = makeiary(n)
   local st = os.clock()
   for i=1,#b do
      a[#a+i-1]=b[i]
   end
   local et = os.clock()
   print("n:",n, "t:",(et-st))
end

doit(1000)
doit(2000)
doit(3000)
doit(4000)
doit(5000)
doit(10000)
doit(20000)
doit(30000)