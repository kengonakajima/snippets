local uv = require("uv")

print("fm:", uv.getFreeMemory() )
print("tm:", uv.getTotalMemory() )
print("avg:", uv.loadavg() )
print("uptime:", uv.uptime() )
p( uv.cpuInfo() )
p( uv.interfaceAddresses() )
print( "path:", uv.execpath() )



--for i=1,1000 do
  print("hr:", uv.hrtime() )
--end
