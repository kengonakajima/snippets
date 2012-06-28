-- Copyright github.com/kengonakajima 2012
-- License: Apache 2.0
-- Compatibility: server: luvit, client: MoaiSDK

-- envs
local luvit, moai = {}, {}

if not _G.lumino then
  _G.lumino = {}
end

pcall( function()
    _G.socket = require("socket")
  end)

if MOAISim then

else
  _G.table = require("table")
  _G.math = require("math")
  _G.dbg = require("debug")
  local res,uv = pcall( function() return require("uv_native") end)
  if res and uv then -- luvit only
    _G.uv = uv
    _G.ffi = require("ffi")
    _G.net = require("net")
    _G.JSON = require("json")
    _G.http = require("http")
    _G.timer = require("timer")
    _G.fs = require("fs")
--    _G.utils = require("utils")
  end
end

-- math and lua values
-- bool to integer
function _G.b2i(b) if b then return 1 else return 0 end end
function _G.nilzero(v) if v == nil then return 0 else return v end end
_G.abs = math.abs
_G.sqrt = math.sqrt
_G.epsilon = 0.000001
function _G.neareq(v1,v2) return (abs(v2-v1) < epsilon) end
function _G.len(x0,y0,x1,y1) return math.sqrt( (x0-x1)*(x0-x1) + (y0-y1)*(y0-y1) ) end
function _G.normalize(x,y,l)
  ll = len(0,0,x,y)
  return x / ll * l, y / ll * l
end
function _G.int(x)
  if not x then
    return 0
  else
    if type(x) == "boolean" then
      return 1
    else
      return math.floor(x)
    end
  end  
end
function _G.int2(x,y) return int(x), int(y) end
function _G.int3(a,b,c) return int(a),int(b),int(c) end
function _G.range(a,b) return a + ( b - a ) * math.random() end
function _G.irange(a,b) return math.floor(range(a,b)) end
function _G.birandom() return (math.random() < 0.5) end
function _G.plusminus(x) if math.random()<0.5 then return x else return x *-1 end end
function _G.avg(a,b) return (a+b)/2 end
function _G.avgs(...)
  local t = {...}
  local total = 0
  for _,v in ipairs(t) do total = total + v end
  return total / #t
end
function _G.sign(x) if x>0 then return 1 elseif x < 0 then return -1 else return 0 end end
-- get between a,b by ratio of aa<=v<=bb
-- get 3 if a,b=1,10 and aa=10,bb=100 and v=30
function _G.ratio(a,b,aa,bb, v)
  local da = v - aa
  local d = bb - aa
  local r = da / d
  return a + (b-a) * r
end

function _G.min(a,b)
  if not a and not b then return 0 end
  if not a then return b end
  if not b then return a end
  if a < b then return a else return b end
end
function _G.max(a,b)
  if not a and not b then return 0 end
  if not a then return b end
  if a > b then return a else return b end
end

-- loop funcs
function _G.times(n,f)
  for i=1,n do f(i) end
end

-- string funcs
_G.byte = string.byte
function _G.split(str, delim)
  assert(delim)
  if string.find(str, delim) == nil then
    return { str }
  end

  local result = {}
  local pat = "(.-)" .. delim .. "()"
  local lastPos
  for part, pos in string.gfind(str, pat) do
    table.insert(result, part)
    lastPos = pos
  end
  table.insert(result, string.sub(str, lastPos))
  return result
end
_G.string.split = split


-- table funcs
_G.join = table.concat
_G.insert = table.insert
_G.remove = table.remove
function _G.shift(t)
  remove(t,1)
end
function _G.merge(to,from)  -- overwrite merge
  for k,v in pairs(from) do
    to[k]=v
  end
  return to
end
function _G.sort(t,f)
  table.sort(t,f)
  return t  
end
function _G.choose(ary) return ary[ int(  math.random() * #ary ) + 1] end
function _G.shuffle(array)
  local arrayCount = #array
  for i = arrayCount, 2, -1 do
    local j = math.random(1, i)
    array[i], array[j] = array[j], array[i]
  end
  return array
end
function _G.find(t,f)
  if type(f)=="function" then
    for i,v in ipairs(t) do
      if f(v) then return v end
    end
  else
    for i,v in ipairs(t) do
      if v==f then return v end
    end
  end
  return nil  
end

function _G.scan(t,f)
  for i,v in ipairs(t) do
    f(v)
  end  
end
_G.each = _G.scan

function _G.filter(t,f)
 local out={}
  for i,v in ipairs(t) do
    local r = f(v)
    if r then 
      table.insert(out, r)
    end
  end
  return out
end
function _G.count(t,f)
  local cnt=0
  for i,v in ipairs(t) do
    if (not f) or f(v) then cnt = cnt + 1 end
  end
  return cnt
end
function _G.keys(t)
  local out = {}
  for k,v in pairs(t) do
    table.insert(out,k)
  end
  return out
end
function _G.keynum(t)
  local cnt=0
  for k,v in pairs(t) do
    cnt = cnt + 1
  end
  return cnt
end
function _G.sumValues(t)
  local tot=0
  for k,v in pairs(t) do
    tot = tot + v
  end
  return tot
end
function _G.table.copy(t)
  local t2 = {}
  for k,v in pairs(t) do
    t2[k] = v
  end
  return t2
end
function _G.table.dupArray(t)
  local out={}
  for i,v in ipairs(t) do
    out[i] = v
  end
  return out  
end
function _G.map(tbl,fncname, ...)
  assert(fncname)
  local out={}
  for k,v in pairs(tbl) do
    if type(v) == "table" then
      local f = v[fncname]
      if type(f) == "function" then
        out[k] = f( v, ... )
      end
    end    
  end
  return out
end
function _G.valcopy( tbl )
  local out ={}
  for k,v in pairs(tbl) do
    if type(v)=="number" or type(v)=="string" or type(v)=="boolean" or type(v) == "boolean" then
      out[k] = v
    end
  end
  return out
end
function _G.slice(t,i1,i2)
  local res = {}
  local n = #t
  -- default values for range
  i1 = i1 or 1
  i2 = i2 or n
  if i2 < 0 then
    i2 = n + i2 + 1
  elseif i2 > n then
    i2 = n
  end
  if i1 < 1 or i1 > n then
    return {}
  end
  local k = 1
  for i = i1,i2 do
    res[k] = t[i]
    k = k + 1
  end
  return res
end

function _G.deepcompare(t1,t2,ignore_mt,eps)
  local ty1 = type(t1)
  local ty2 = type(t2)

  if ty1 ~= ty2 then return false end
  -- non-table types can be directly compared
  if ty1 ~= 'table' then
    if ty1 == 'number' and eps then return abs(t1-t2) < eps end
    return t1 == t2
  end
  return true
end


function _G.dumpbytes(str)
  local out={}
  for i=1,#str do
    insert(out, str:byte(i,i))
  end
  return table.concat(out," ")
end
_G.string.dumpbytes = dumpbytes
function _G.dumphex(str)
  local out={}
  for i=1,#str do
    insert(out, string.format("%02x",str:byte(i,i)))
  end
  return table.concat(out," ")  
end
_G.string.dumphex = dumphex
function _G.trim(s)
  return (s:gsub("^%s*(.-)%s*$", "%1"))
end
_G.string.trim = trim

-- return value nearer to __from__
function _G.nearer(from,a,b)
  local da = math.abs(a-from)
  local db = math.abs(b-from)
  if da < db then
    return a
  else
    return b
  end   
end

-- log funcs
function _G.dump1(s,t)
  for k,v in pairs(t) do print( s, k,v ) end
end
function _G.printTrace(erro)
  print( erro )
  print( debug.traceback(100) )
end
function _G.sprintf(...) return string.format(...) end
function _G.printf(...)
  io.stdout:write( sprintf(...) )
  io.stdout:flush()  
end 
function _G.prt(...)
  local s = table.concat({...}," ")
  io.stdout:write(s)
  io.stdout:flush()
end
function _G.datePrint(...)
  local s = table.concat({...}," ")
  io.stdout:write( "[" .. os.date() .. "] " .. s .. "\n" )
  io.stdout:flush()  
end
function _G.dump(t)
  for i=1,#t do
    print( "["..i.."]", t[i])
  end
  for k,v in pairs(t) do
    print(k,v)
    if type(v) == "table" then
      dump(v)
    end
  end
end
function _G.num2digitString(n)
  local s
  if n == 1 then
    s = ""
  elseif n < 10 then
    s = string.format( " %d", n )
  else
    s = string.format( "%d", n )
  end
  return s
end
function _G.appendLog(path,s)
  local fp = assert(io.open(path,"a+"))
  fp:write("["..os.date().."] "..s.."\n")
  fp:close()
end
function _G.TextTable(w,h,default)
  local t = {
    data = {},
    w = w,
    h = h
  }
  
  for y=1,h do
    t.data[y] = {}
    for x=1,w do
      t.data[y][x] = default
    end
  end
  function t:getChar(x,y)
    return self.data[int(y)][int(x)] 
  end  
  function t:setChar(x,y,c)
    self.data[int(y)][int(x)] = c    
  end
  function t:toString()
    local ary = {}
    for y=self.h-1,1,-1 do
      table.insert( ary, table.concat( self.data[y] ) )
    end
    return table.concat( ary, "\n" )
  end  
  
  return t    
end



-- timer funcs
_G.now = os.time -- overwritten by UV and MOAI on last merge
function _G.measure( f )
  local st = now()
  f()
  local et = now()
  return (et-st)
end

if not uv then 
  _G.laterCalls={}
  function _G.later(latency,f)
    table.insert( laterCalls, { callAt = now() + latency, func = f } )
  end

  function _G.pollLater()
    local nt = now()
    for i,v in ipairs(laterCalls) do
      if v.callAt < nt then
        v.func()
        remove(laterCalls,i)
      end    
    end  
  end
else
  function _G.later(t,f)
    timer.setTimeout(t*1000,f)
  end  

end


-- file funcs
function _G.readFile(path)
  local ok, data = pcall( function()
      local fp = io.open(path)
      local s = fp:read(1024*1024*1024)
      fp:close()
      return s
    end)
  if ok then return data else return nil end
  return s
end
function _G.writeFile(path,data)
  local ok, ret = pcall( function()
      local fp = io.open(path,"w")
      local ret = fp:write(data)
      fp:close()
      return ret
    end)
  if ok then return ret else return nil end
end
function _G.existFile(fn)
  local f = io.open(fn)
  if not f then
    return false
  else
    f:close()
    return true
  end
end

-- json funcs
function _G.readJSON(path)
  local s = readFile(path)
  if s then
    return JSON.parse(s)
  else
    return nil
  end
end
function _G.writeJSON(path,t)
  return writeFile(path, JSON.stringify(t))
end
function _G.mergeJSONs(...)
  local paths={...}
  local out={}
  local foundAny=false
  for i,path in ipairs(paths) do
    local t = readJSON(path)
    if t then
      merge(out,t)
      foundAny = true
    end    
  end
  if not foundAny then return nil else return out end
end


-- csv funcs
function _G.loadTableFromCSV(fn)
  if existFile(fn) then
    local csv = readCSV(fn)
    return csvToTable(csv)
  else
    return {}
  end
end

function _G.saveTableToCSV(fn,t)
  local csv={}
  local cnt=1
  for k,v in pairs(t) do
    table.insert( csv, { [1]=k,[2]=v } )
  end
  writeCSV(fn,csv)
end

-- csv: array of array
function _G.csvToTable(csv)
  local out={}
  for i,row in ipairs(csv) do
    out[ row[1] ] = row[2]
  end
  return out
end

function _G.readCSV(file)
  local fp = assert(io.open (file))
  local csv = {} 
  for line in fp:lines() do
    if not line then break end
    local row = split(line,",")
    csv[#csv+1] = row
  end
  fp:close()
  return csv
end

function _G.writeCSV(file,tab)
  local fp = assert(io.open(file,"w"))
  for i,row in ipairs(tab) do
    for i,value in ipairs(row) do
      fp:write(tostring(value)..",")
    end
    fp:write "\n"
  end
  fp:close()
end


