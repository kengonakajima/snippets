a={5,6,2,6,2,34,5,5 }

for i,v in ipairs(a) do
   print("i,v:", i,v )
   table.remove( a, i )
end
