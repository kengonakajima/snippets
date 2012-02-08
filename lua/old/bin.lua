a={0,1,2,3,  10,50,100,126,  127,128,129,200,  254,255,256,257 }
s=""
print("num of a:", #a)
f=io.open("out","wb")
for i=1,#a do
   s = s .. string.char(a[i]%256)
end
f:write(s)
f:close()

f=io.open("out","rb")
s=f:read(99999)
f:close()
print("len::", string.len(s))

a={}
l = string.len(s)
for i=1,l do
   a[i] = s:byte(i)
end
for i=1,#a do
   print( "a[",i,"]=",  a[i] )
end
