Vector3={}
function Vector3.new(x,y,z)
   local o = { x=x,y=y,z=z}
   return setmetatable(o, {__index=Vector3})
end

function Vector3:translate(v)
   self.x = self.x + v.x
   self.y = self.y + v.y
   self.z = self.z + v.z
   return self
end

ary,ary1 = {},{}

NVEC=10000
NLOOP=10000
-- init
for i=1,NVEC do
   ary[i] = Vector3.new(0,0,0)
   ary1[i] = Vector3.new(1,1,1)
end

-- loop
for i=1,NLOOP do
   for j=1,NVEC do
      ary[j]:translate( ary1[j] )
   end
end
v=ary[NLOOP/2]
print( v.x, v.y, v.z )
