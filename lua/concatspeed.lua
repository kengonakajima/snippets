function makeiary(n)
   local out={}
   for i=1,n do table.insert(out,i) end
   return out
end

ary = makeiary(100000)

function doit(len,loop)   
   local st = os.clock()
   local s
   for i=1,loop do
      s = table.concat(ary, "",1,len )
   end   
   local et = os.clock()
   print("len:",len, "loop:",loop, "time:",(et-st), "q/sec:", (loop/(et-st)))
   return s
end
print( table.concat( { 1.1, 2.2 }, "," ) )

doit(100,100)
doit(500,500)
doit(1000,10)
doit(10000,10)
doit(20000,10)
doit(30000,10)
doit(50000,10)
doit(100000,10)