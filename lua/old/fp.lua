fp = io.open("fp.lua","rb")
for i=1,100 do
   b = fp:read(2)
   if not b then
      break
   end
   print("b:", #b, b)

end
fp:close()
