a = {1,2,3,4,5,6,7,8,9,8,7,6,5,4,3,2,1}
b = {2,9,2,9,2,9,2,9,2,9,2,9,2,9,2,9,2}
c = {}
for i,v in ipairs(a) do
   c[i] = a[i] + b[i]
end

function lowpass(data,alpha)
   local out = {}
   local y_old = 0
   for i,x in ipairs(data) do
      local y = alpha * x + ( 1.0 - alpha ) * y_old
      out[i] = y
      y_old = y
   end
   return out
end

   
data = lowpass(c,0.5)
for i,v in ipairs(data) do
   print( i,v )
end
