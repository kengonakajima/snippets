require("gettimeofday")

function makeCell()
  local out = {}
  out.height = 0
  out.water = 0
  out.temperature = 0
  return out
end


local fld = {}

local st = gettimeofday()
for y=0,1024 do
  fld[y] = {}
  for x=0,1024 do
    fld[y][x] = makeCell()
  end
end
local et = gettimeofday()

print("init:", et-st)



st = gettimeofday()
for poo=1,10 do
  for y=0,1024 do
    for x=0,1024 do
      local cell = fld[y][x]
      cell.height = cell.height + 1
      cell.water = cell.water + 1
      cell.temperature = cell.height + cell.water
    end
  end
end

et = gettimeofday()

print("proc:", et-st)  # 20 times faster than creation.

