-- compare 2d ary speed
require("gettimeofday")

local st = gettimeofday()

for poo=1,100 do
  local ary1d = {}
  for i=0,1024 do
    for j=0,1024 do
      local index = j + i*1024
      ary1d[index] = i+j+poo
    end
  end
end

local et = gettimeofday()

print( "1d:", et-st)

st = gettimeofday()
for poo=1,100 do
  local ary2d = {}
  for i=0,1024 do
    ary2d[i] = {}
    for j=0,1024 do
      ary2d[i][j] = i+j+poo
    end
  end
end
et = gettimeofday()

print( "2d:", et-st )   -- about 4 times faster