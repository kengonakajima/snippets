table=require("table")

a="abcdefghijklmnopqrstuvwxyz abcdefghijklmnopqrstuvwxyz abcdefghijklmnopqrstuvwxyz"

function divideData(orig,unit)
  local out={}
  local startAt=1
  while true do
    local endAt = startAt + (unit-1)
    local s = orig:sub(startAt,endAt)
    if #s > 0 then
      table.insert(out,s)
    else
      break
    end    
    startAt = startAt + unit
  end
  return out
end

print(#a)

t=divideData(a,26)
for i,v in ipairs(t) do
  print(i, #v, v )
end
