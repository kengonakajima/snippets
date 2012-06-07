a="a.b.c"
print(a:find(".",1,true))

for s in a:gmatch( ".",1,true) do
  print(s)
end

