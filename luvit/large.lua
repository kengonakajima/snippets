require( "./lumino/lumino" )

function _G.C(n)
  local obj={}
  obj.a = n
  obj.b = n+1
  obj.c = n*2
  obj.d = n+2
  obj.e = n*3
  obj.f = n+4
  obj.g = n*4
  obj.h = n+5
  obj.i = n*5
  obj.j = n+6
  obj.k = n*8
  return obj
end

n = 100 * 1000

t = {}
for i=1,n do
  local c = C(i)
  insert( t, c )
end

every( 0.1, function()
         print( collectgarbage( "count" ) )
         local tot =  0
         for i=1,n do
           t[i] = C(i)
         end         
         print( "loop..", tot )
          end)

