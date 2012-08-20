require("string")
require("./lumino/lumino")

function to_s(t)
  return string.format( "%d:%d:%d:%d:%d:%d:%d:%d:%d:%d", t.a,t.b,t.c,t.d,t.e, t.f,t.g,t.h,t.i,t.j)
end

function to_b(t)  
  return string.char(t.a, t.b, t.c, t.d, t.e, t.f, t.g, t.h, t.i, t.j )
end

local vals={}

for i=1,10000 do
  local k = i % 256
  insert( vals, {a=k,b=k,c=k,d=k,e=k, f=k,g=k,h=k,i=k,j=k} )
end


t = measure(function()
    for j=1, 100 do
      local out={}
      for i,v in ipairs(vals) do
        local s = to_s(v)
        insert(out,s)
      end
      local os = join(out)
      print( "outlen:", #os, j )
    end
  end)

print("total time:", t )

-- to_s : 10000 x 100 で3秒 @ luvit
-- to_b : 10000 x 100 で0.6秒 @ luvit