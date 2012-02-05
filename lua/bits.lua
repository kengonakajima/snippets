function byte_and(x)
   if x < 0 then
      x = x + 256
   end
   return (x%256)
end

print( 0,byte_and(0) )
print( 1,byte_and(1) )
print( 2,byte_and(2) )
print( 127,byte_and(127) )
print( 128,byte_and(128) )
print( 129,byte_and(129) )
print( 254,byte_and(254) )
print( 255,byte_and(255) )
print( 256,byte_and(256) )
print( -1,byte_and(-1) )
print( -2,byte_and(-2) )
print( -127,byte_and(-127))
print( -128,byte_and(-128))
print( -129,byte_and(-129))
print( -254,byte_and(-254) )
print( -255,byte_and(-255) )
print( -256,byte_and(-256) )

function byte_rshift(x,n)
   
end
