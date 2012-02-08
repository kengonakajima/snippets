luabit = require("./luabit")

-- 0.1を　　{0,0,0,1,1,0,0,0,0,0,1,1,0,0...}に変換する
function tobits(x,n)
   local out = {}
   for i=1,n do
      local nx = x * 2
      local nextbit
      if nx >= 1 then
         nextbit = 1
      else
         nextbit = 0
      end
      table.insert(out, nextbit )
         
      if nextbit == 1 then
         nx = nx - 1
      end
      x = nx
   end
   return out
end

hoge = tobits( 0.1, 24 )
s=""
for k,v in pairs(hoge) do
   s = s .. v .. " "
end
print(s)

-- 逆変換をする
function frombits(ary)
   local x = 0
   local cur = 0.5
   for i,v in ipairs(ary) do
      x = x + cur * v
      cur = cur / 2
   end
   return x   
end

xx = frombits(hoge)

print("xx:",xx)

-- 0001 1001 1001 1001 1001

-- byte to bits

function tobits(ary)
   local out={}
   for i,v in ipairs(ary) do
      for j=0,7,1 do
         table.insert(out, luabit.band( luabit.brshift(v,7-j), 1 ) )
      end
   end
   return out
end

bb = tobits( { 0x19,0x99,0x99 } )
s=""
for k,v in pairs(bb) do
   s = s .. v .. " "
end
print(s)

print( "last:",  frombits(bb) )