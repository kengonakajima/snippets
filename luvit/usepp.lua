require "./lumino/lumino"
require "./pp"

profiler = newProfiler("time",1)
profiler:start()

local tot = 0
for i=1,10000 do
  tot = tot + i
end
print("total:", tot)

profiler:stop()

local outfile = io.open( "/tmp/profile.txt", "w" )
profiler:report( outfile )
outfile:close()
print( readFile("/tmp/profile.txt"))

