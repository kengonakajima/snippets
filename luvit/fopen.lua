--
require("./lumino/lumino" )

local s = "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789" .. "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789" .. "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789" .. "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789" .. "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789" .. "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789" .. "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789" .. "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789" .. "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789" .. "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789" .. "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789" .. "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789" .. "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789" .. "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789" .. "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789" .. "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789" .. "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789" .. "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789" .. "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789" .. "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789" .. "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789" .. "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789" .. "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789" .. "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789" .. "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789" .. "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789" .. "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789" .. "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789" .. "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789" .. "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789" .. "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789" .. "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789" .. "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789" .. "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789" .. "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789" .. "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789" .. "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789" .. "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789" .. "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789" .. "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"

t=measure(function()
    for i=1,1000 do
      writeFile( "/tmp/tmp_"..i,s)
    end    
  end)

print("ttttttt:",t )

-- ssdの場合は 1000回で 0.18sec 