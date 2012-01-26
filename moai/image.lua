-- to read map data from png

img = MOAIImage.new ()
img:load ( "./sol.png" )

w, h = img:getSize ()

print( "image size:", w,h )

function hex(x)
   return math.floor( x * 255 )
end

for y=1,h do
   for x=1,w do
      local r,g,b,a = img:getRGBA(x,y)
      r,g,b,a = hex(r),hex(g),hex(b),hex(a)
      print( string.format( "(%d,%d) %02x %02x %02x %02x", x,y,r,g,b,a))
   end
end

   