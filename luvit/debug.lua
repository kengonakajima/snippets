require("./lumino/lumino")

debug = require("debug")


calls ={}


function hookfunc(p)
  local fn = debug.getinfo(2).name
  if not calls[fn] then calls[fn]=0 end
  calls[fn] = calls[fn]+1
end

debug.sethook( hookfunc, "c")


function hoge()
  return 1
end


local tot=0
for i=1,1000000 do
  tot = tot + hoge()
end
print(tot)


pp(calls)

