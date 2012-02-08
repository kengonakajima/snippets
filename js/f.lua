function fff(x)
   return x + 2
end
function ggg(x)
   local a={}
   a.p = x
   return a.p
end

t=0
for i=0,100000000 do
   t = t + fff(i)
end

print("ttt:"..t)
