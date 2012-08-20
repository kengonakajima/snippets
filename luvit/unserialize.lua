-- convert bin to table

require("string")
require("./lumino/lumino")

local origs = {}
for i=1,10000 do
--  insert(origs, string.char( i%256,i%256,i%256,i%256,i%256, i%256,i%256,i%256,i%256,i%256 ) )
  insert(origs, string.format( "%d:%d:%d:%d:%d:%d:%d:%d:%d:%d", i%256,i%256,i%256,i%256,i%256, i%256,i%256,i%256,i%256,i%256 ))
end

t = measure(function()
    for i=1,100 do
      local tot = 0      
      for j,v in ipairs(origs) do
--        local t = {v:byte(1,10)}
        local ts = split(v,":")
        tot = tot + tonumber(ts[1])+tonumber(ts[2])+tonumber(ts[3])+tonumber(ts[4])+tonumber(ts[5])+tonumber(ts[6])+tonumber(ts[7])+tonumber(ts[8])+tonumber(ts[9])+tonumber(ts[10])
        
--        tot = tot + t[1] + t[2] + t[3] + t[4] + t[5] + t[6] + t[7] + t[8] + t[9] + t[10]
      end
      print("tot:",tot, i )
    end
  end)

print("total time:", t )

-- byteをつかうと　　0.43sec
-- splitだと   6.28 sec 15倍
  
