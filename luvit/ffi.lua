require("./lumino/lumino")

local ffi = require("ffi")


ffi.cdef[[
    int getpid();
    int system( const char *cmd );
]]

print( ffi.C.getpid() )
--print( ffi.C.system( "ls /etc" ) )

-- prefix: "/tmp/hoge_"
function getGUID()
  if not _G.lumino.guid then
    _G.lumino.guid = 1
  end
  lumino.guid = lumino.guid + 1
  local t = int(now())
  local pid = 0
  if getpid then pid = getpid() end
  return "" .. t .. "_" .. pid .. "_" .. lumino.guid
end

function makeTmpPath(prefix)
  assert(prefix)
  return prefix .. "_" .. getGUID()
end

function cmd(s)
  local tmp = makeTmpPath("/tmp/lumino_cmd")
  local s = ffi.C.system( sprintf( "%s > %s 2>/dev/null", s, tmp ) )
  local res = readFile(tmp)
  return res
end
print( makeTmpPath("/tmp/hoge"))
print( measure(function()
for i=1,100 do
  s = cmd( "ls -t /etc" )
  ary = split(s,"\n")
  prt( #ary , " ")
end
end))