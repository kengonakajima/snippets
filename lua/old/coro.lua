function hoge(n)
   for i=1,n do
      print("counter:",i )
      coroutine.yield(true)
   end
   while true do
      coroutine.yield(nil)
   end   
end
local f = coroutine.wrap( hoge )

f(10)
for i=1,20 do
   local ret = f()
   if not ret then
      print("finished")
      break
   end
end




