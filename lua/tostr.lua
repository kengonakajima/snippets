-- fast string concatenator by binary tree
function tabletostring(t)
   assert(#t>0)

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

t = { "a","b","c","d" }

s = tabletostring(t)

print(s)
