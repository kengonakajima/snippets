-- string append slow?

function tabletostring(t)
   local newt={}
   if #t == 1 then
      return t[1]
   end
   
   if (#t % 2) == 1 then
      t[#t-1] = t[#t-1] .. t[#t]
      table.remove(t, #t )
   end
   for i=1,#t,2 do
--      print("iterate. i:",i)
      table.insert(newt, t[i] .. t[i+1] )
   end
   return tabletostring(newt)
end

function tostrary(ary)
   for i,v in ipairs(ary) do
      ary[i] = string.char(ary[i])
   end
   return ary
end

function numarytostring(ary)
   return tabletostring( tostrary(ary) )
end

function table_slice (values,i1,i2)
   local res = {}
   local n = #values
   -- default values for range
   i1 = i1 or 1
   i2 = i2 or n
   if i2 < 0 then
      i2 = n + i2 + 1
   elseif i2 > n then
      i2 = n
   end
   if i1 < 1 or i1 > n then
      return {}
   end
   local k = 1
   for i = i1,i2 do
      res[k] = values[i]
      k = k + 1
   end
   return res
end

a={ 90, 65,66,67,68, 69,70,71,72, 73, 90,90 }
s = numarytostring( table_slice(a,2,2+8) )
print(s)


n = 180000
print("create")
a = {}
for i=1,n do
   table.insert(a, 65 + (i%20) )
end
print("convert")
st = os.clock()
s = numarytostring(a)
et = os.clock()


print("time:", (et-st), #s )