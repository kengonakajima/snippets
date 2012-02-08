function aho(a,b,...)
   local t = {...}
   print("tn:", #t )
   for i in pairs(t) do
      print("i",i)
   end
end



aho(1,2,3,4,5)