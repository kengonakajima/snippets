a={1,2,3,[100]=200,[101]=333}

for i,v in ipairs(a) do
  print(i,v)
end

for k,v in pairs(a) do
  print(k,v)
end
