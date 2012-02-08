f=io.open("/usr/local/bin/lua")
s=f:read(9999999)
f:close()
l=string.len(s)
ary={}
n=10
for j=1,n do
   for i=1,l do
      ary[i]=string.char( s.byte(i) )
   end
end

   