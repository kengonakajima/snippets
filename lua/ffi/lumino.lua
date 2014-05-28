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
  _G.io = require("io")
  _G.string = require("string")
  _G.os = require("os")
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
    _G.bit = require("bit")
    _G.querystring = require("querystring")
    _G.luvitutils = require("utils")
    _G.spawn = require("childprocess").spawn
  end
end

-- math and lua values
-- bool to integer
function _G.isnan(n) return ( n ~= n ) end 
function _G.b2i(b) if b then return 1 else return 0 end end
function _G.nilzero(v) if v == nil then return 0 else return v end end
_G.abs = math.abs
_G.sqrt = math.sqrt
_G.epsilon = 0.000001
function _G.neareq(v1,v2) return (abs(v2-v1) < epsilon) end
function _G.len(x0,y0,x1,y1) return math.sqrt( (x0-x1)*(x0-x1) + (y0-y1)*(y0-y1) ) end
function _G.normalize(x,y,l)
  local ll = len(0,0,x,y)
  if ll==0 then return 0,0 end
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
function _G.to_i(s)
  if type(s) == "string" then
    return math.floor( tonumber(s) )
  else
    return int(s)
  end  
end
function _G.to_f(s)  return tonumber(s) end
function _G.to_s(n)  return tostring(n) end
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
  if not b then return a end   
  if a > b then return a else return b end
end
function _G.ternary(cond,t,f)
  if(cond) then return t else return f end
end
_G.cond = ternary

-- lua typeof
_G.typeof = type

-- guards
function _G.muststring(s)
  assert( typeof(s)=="string", "not a string" )
  return s
end
function _G.mustnumber(n)
  assert( typeof(n)=="number", "not a number" )
  return n
end
function _G.musttable(t)
  assert( typeof(t)=="table", "not a table" )
  return t
end

-- loop funcs
function _G.times(n,f)
  for i=1,n do f(i) end
end


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
  if f then
    table.sort(t,f)
  else
    table.sort(t, function(a,b) return a<b end )
  end
  return t  
end
function _G.choose(ary)
  if not ary then return nil end
  return ary[ int(  math.random() * #ary ) + 1]
end
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

function table.select(t,f)
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
  for i,v in pairs(t) do
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
function _G.values(t)
  local out = {}
  for k,v in pairs(t) do
    table.insert(out,v)
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
function _G.table.reverse(t)
  local out={}
  for i,v in ipairs(t) do
    out[#t-(i-1)] = t[i]
  end
  return out  
end
_G.reverse = _G.table.reverse
-- slide left to right
function _G.table.slide(t)
  local out={}
  for i=#t,2,-1 do
    out[i]=t[i-1]
  end
  out[1]=t[#t]
  return out
end
_G.slide = _G.table.slide
function _G.table.unshift(t,val)
  local out={}
  out[1] = val
  for i=1,#t do
    out[i+1]=t[i]
  end
  return out
end
_G.unshift = _G.table.unshift

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
  -- as well as tables which have the metamethod __eq
  local mt = getmetatable(t1)
  if not ignore_mt and mt and mt.__eq then
    return t1 == t2
  end
  for k1,v1 in pairs(t1) do
    local v2 = t2[k1]
    if v2 == nil or not deepcompare(v1,v2,ignore_mt,eps) then
      return false
    end
  end
  for k2,v2 in pairs(t2) do
    local v1 = t1[k2]
    if v1 == nil or not deepcompare(v1,v2,ignore_mt,eps) then
      return false
    end
  end
  return true
end

-- remove data types (recurse)
function _G.removeTypes(t,rmtype)
  local out={}
  for k,v in pairs(t) do
    local tname = typeof(v)
    if tname == rmtype then
    else
      if tname == "table" then
        out[k] = _G.removeTypes(t[k],rmtype)
      else
        out[k] = t[k]
      end      
    end    
  end
  return out
end

-- recursively tonumber(key)
function _G.table.tonumber(t)
  local out={}
  for k,v in pairs(t) do
    local n = tonumber(k)
    if n then
      if typeof(v)=="table" then
        out[n]=table.tonumber(v)
      else
        local nv = tonumber(v)
        if nv then
          out[n]=nv
        else
          out[n]=v
        end        
      end
    else
      if typeof(v)=="table" then
        out[k]=table.tonumber(v)
      else
        local nv = tonumber(v)
        if nv then
          out[k]=nv
        else
          out[k]=v
        end
      end
    end    
  end
  return out
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

function _G.utf8div(str)
  local t={}
  for c in str:gmatch("[\1-\127\192-\255][\128-\191]*") do
    table.insert(t,c)
  end
  return t  
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

function _G.string.divide(orig,unit)
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
function _G.isalpha(str)
  for i=1,#str do
    local b = str:byte(i)
    if (b >= 65 and b <= 90) or ( b >= 97 and b <= 122 ) then
      -- ok
    else
      return false
    end
  end
  return true
end


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
  if erro then
    print( erro )
    local s = require("debug").traceback(100) 
    print(s)
  else
    local s
    xpcall( function() caused_by_printTrace() end,
      function(e)
        s = require("debug").traceback(e)
        print("LOG:", s)
      end)
    return s
  end  
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
  -- timers on moai: usage: later|every(delay,f); startTimerPoller() or call pollLater() in your app
  _G.laterCalls={}  
  function _G.later(latency,f)
    table.insert( laterCalls, { callAt = now() + latency, func = f } )
  end
  function _G.every(interval,f)
    local wrap = function()
      f()
      every( interval, f )
    end
    table.insert( laterCalls, { callAt = now() + interval, func = wrap } )
  end
  function _G.startTimerPoller()
    local t = MOAIThread.new()
    t:run( function()
        while true do
          pollLater(now())
          coroutine.yield()
        end        
      end)
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

function _G.nowi()
  return int(now())
end  

  
-- file funcs
function _G.readFile(path)
  local ok, data = pcall( function()
      local fp = io.open(path,"rb")
      local s = fp:read(1024*1024*1024)
      fp:close()
      return s
    end)
  if ok then return data else return nil end
end
function _G.writeFile(path,data)
  local ok, ret = pcall( function()
      local fp = io.open(path,"wb")
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
-- expect file exists
-- ofs: start from 0
function _G.overwriteFileOffset(path,data,ofs)
  local ok, ret = pcall( function()
      local fp = io.open(path,"r+b")
      fp:seek("set",ofs)
      local ret = fp:write(data)
      fp:close()
      return ret
    end)
  if ok then return ret else return nil end
end


-- json funcs
if _G.JSON then
  local origParse = JSON.parse
  function JSON.parse(s)
    local ok,ret=pcall(function()
        return origParse(s)
      end)
    if not ok then return nil else return ret end
  end

  function _G.readJSON(path)
    local s = readFile(path)
    if s then
      local t = JSON.parse(s)
      return table.tonumber(t)
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



-- 2D/3D funcs
function _G.randomize2D(x,y,r) return x + ( -r  + math.random() * (r*2) ), y + ( -r  + math.random() * (r*2) ) end
_G.randomize = _G.randomize2D
function _G.expandVec(fromx,fromy,tox,toy,tolen)
  local dx,dy = (tox-fromx),(toy-fromy)
  local nx,ny = normalize(dx,dy,tolen)
  return fromx + nx, fromy + ny  
end
function _G.len3d(x0,y0,z0,x1,y1,z1)   return math.sqrt( (x0-x1)*(x0-x1) + (y0-y1)*(y0-y1) + (z0-z1)*(z0-z1) ) end
function _G.vec3len(va,vb) return math.sqrt( (va.x-vb.x)*(va.x-vb.x) + (va.y-vb.y)*(va.y-vb.y) + (va.z-vb.z)*(va.z-vb.z) ) end
function _G.vec3sub(va,vb) return { x = vb.x-va.x, y = vb.y-va.y, z = vb.z-va.z } end
function _G.vec3eq(va,vb) return ( va.x==vb.x and va.y==vb.y and va.z==vb.z) end
function _G.vec3cross(v1,v2)
  local x1,y1,z1 = v1.x, v1.y, v1.z
  local x2,y2,z2 = v2.x, v2.y, v2.z
  return { 
    x = y1 * z2 - z1 * y2,
    y = z1 * x2 - x1 * z2,
    z = x1 * y2 - y1 * x2
  }  
end
function _G.vec3dot(v1,v2) return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z end
function _G.vec3normalize(v)
  local l = math.sqrt( v.x*v.x + v.y*v.y + v.z*v.z )
  if l==0 then return vec3(0,0,0) end
  return vec3(v.x/l, v.y/l, v.z/l)
end
function _G.vec3(x,y,z) return {x=x,y=y,z=z} end
function _G.vec3toString(v) return string.format("(%f,%f,%f)", v.x, v.y, v.z ) end
function _G.vec3toIntString(v) return string.format("(%d,%d,%d)", v.x, v.y, v.z ) end
-- t, u,v 
function _G.triangleIntersect( orig, dir, v0,v1,v2 )
  local e1 = vec3sub(v1,v0)
  local e2 = vec3sub(v2,v0)
  local pvec = vec3cross(dir,e2)
  local det = vec3dot(e1,pvec)
  
  local tvec = vec3sub(orig,v0)
  local u = vec3dot(tvec,pvec)
  local qvec
  local v
  if det > 1e-3 then
    if u < 0 or u > det then return nil end
    qvec = vec3cross(tvec,e1)
    v = vec3dot(dir,qvec)
    if v < 0 or u+v >det then return nil end
  elseif det < - 1e-3 then
    if u > 0 or u < det then return nil end
    qvec = vec3cross(tvec,e1)
    v = vec3dot(dir,qvec)
    if v > 0 or u+v <det then return nil end
  else
    return nil
  end
  local inv_det = 1 / det  
  local t = vec3dot(e2,qvec)
  t = t * inv_det * -1
  u = u * inv_det
  v = v * inv_det
  return t,u,v
end


-- rect funcs
function _G.Rect(minx,miny,maxx,maxy)
  local r = { minx=minx, maxx=maxx, miny=miny,maxy=maxy }
  function r:include(x,y) return ( x >= self.minx and x <= self.maxx and y >= self.miny and y <= self.maxy ) end
  function r:toData() return removeTypes(self, "function" ) end
  return r
end
function _G.squareRect(cx,cy,w)
  assert(cx and cy and w )
  return Rect( cx-w,cy-w,  cx+w,cy+w )
end

-- sz from center
function _G.makeHitRect(prop, sz )
  assert(sz)
  local x,y = prop:getLoc()
  return Rect( x - sz, y - sz, x + sz, y + sz )
end
function _G.checkHitRects(rects, x,y,arg, cb)
  for i,v in ipairs(rects) do
    if x >= v.hitRect.minx and x <= v.hitRect.maxx and y >= v.hitRect.miny and y <= v.hitRect.maxy then
      if cb then
        cb(v,x,y,arg)
      elseif v.callback then
        v:callback(x,y,arg)
      end
    end
  end  
end
function _G.scanCircle(cx,cy,dia,step,fn)
  local rdia = int(dia*1.41)
  for x=cx-rdia,cx+rdia,step do
    for y=cy-rdia,cy+rdia,step do
      if len( cx,cy,x,y) < dia then
        fn(x,y)
      end         
    end
  end  
end
function _G.scanRect(x0,y0,x1,y1,fn)
  for y=y0,y1 do
    for x=x0,x1 do
      fn(x,y)
    end
  end
end


-- 4dirs
_G.DIR = { UP=0,RIGHT=1,DOWN=2,LEFT=3 }
_G.DIRS = { DIR.UP, DIR.RIGHT, DIR.DOWN, DIR.LEFT }
_G.DIR2CHAR = { [DIR.UP]="^", [DIR.DOWN]="v", [DIR.LEFT]="<", [DIR.RIGHT]=">"}
function _G.dir2char(d)
  local out = DIR2CHAR[d]  
  if not d or not out then return "+" end
  return out
end
function _G.randomDir() return choose( DIRS ) end

function _G.reverseDir(d)
  if d == DIR.UP then
    return DIR.DOWN
  elseif d == DIR.DOWN then
    return DIR.UP
  elseif d == DIR.LEFT then
    return DIR.RIGHT
  elseif d == DIR.RIGHT then
    return DIR.LEFT
  else
    assert(false)
  end   
end

-- poly: assuming aabb {x,y,x,y,x,y,x,y}
function _G.calcArea( poly )
  x0,y0,x1,y1,x2,y2,x3,y3 = poly[1],poly[2],poly[3],poly[4],poly[5],poly[6],poly[7],poly[8]
  leftx = min( min(x0,x1), min(x2,x3) )
  rightx = max( max(x0,x1), max(x2,x3) )
  bottomy = min( min(y0,y1), min(y2,y3) )
  topy = max( max(y0,y1), max(y2,y3) )
  return ( rightx - leftx ) * ( topy - bottomy )
end




-- networking and DB
function _G.sendmail( from, to, subj, msg )
  assert(msg)
  local cl
  cl = net.createConnection(25, '127.0.0.1', function (err)
      if err then error(err) end

      print("connected")

      cl.got = ""
      cl:on("data", function(data)
          cl.got = cl.got .. data
          local lines = split( cl.got, "\r\n" )
          for i,l in ipairs(lines) do
            local ary = split(l, " " )
            if ary[1] == "221" then
              print("finished")
              native.close( cl._handle ) 
            end
          end
        end)
      cl:on("error", function(e)
          print("error:",e)
        end)
        
      cl:write( "HELO localhost.localdomain\r\n" )

      cl:write( "EHLO localhost.localdomain\r\n")
      cl:write( "MAIL FROM:<" .. from .. ">\r\n") 
      cl:write( "RCPT TO:<" .. to .. ">\r\n") 
      cl:write( "DATA\r\n")                 
      cl:write( "Subject: " .. subj .. "\r\n") 
      cl:write( "From: " .. from .. "\r\n")   
      cl:write( "Content-type: text/plain; charset=iso-2022-jp\r\n")
      cl:write( "Sender: " .. from .. "\r\n" )
      cl:write( "Date: " .. os.date() .. "\r\n" )
      cl:write( "To: " .. to .. "\r\n" )
      cl:write( "\r\n" )
      cl:write( "\r\n" )
      cl:write( msg )
      cl:write( "\r\n" )      
      cl:write( ".\r\n" )
      cl:write( "QUIT\r\n" )
    end)
end


function _G.isByteUsableInName(b)
  -- 0-9 A-Z a-z _ -
  return (b >= 48 and b <= 57) or (b>=65 and b<=90) or (b>=97 and b<=122) or b==95 or b==45
end
function _G.isUsableInName(str)
  for i=1,#str do
    if not isByteUsableInName(str:byte(i,i)) then
      return false
    end    
  end
  return true
end
  
_G.globalCounterGen = 0
function _G.generateNewId()
  globalCounterGen = globalCounterGen + 1
  return globalCounterGen
end



function _G.getGUID()
  if not _G.lumino.guid then
    _G.lumino.guid = 1
  end
  lumino.guid = lumino.guid + 1
  local t = int(now())
  local pid = 0
  if getpid then pid = getpid() end
  return "" .. t .. "_" .. pid .. "_" .. lumino.guid
end

function _G.makeTmpPath(prefix)
  assert(prefix)
  return prefix .. "_" .. getGUID()
end


-- UNIX funcs
if ffi then 
  ffi.cdef[[
      int getpid();
      int sigignore(int sig);
      int kill(int pid, int sig);
      void (*signal(int sig, void (*func)(int)))(int);
      int unlink(const char *path);

      struct timeval {
        long  tv_sec;   
        long  tv_usec;  
      };    
      int gettimeofday(struct timeval *restrict tp, void *restrict tzp);
      int mkdir( const char *path, int mode );
      int system( const char *cmd );      
  ]]
  function _G.now()
    local tmv = ffi.new( "struct timeval")
    ffi.C.gettimeofday( tmv, nil )
    return tonumber(tmv.tv_sec) + tonumber(tmv.tv_usec) / 1000000
  end

  function _G.cmd(s,cb)
    local tmp = makeTmpPath("/tmp/lumino_cmd")
    local ret = ffi.C.system( sprintf( "%s > %s 2>/dev/null", s, tmp ) )
    local res = readFile(tmp)
    unlink(tmp)
    if not res and ret == 0 then -- lua io sucks: it returns nil if the file is empty.... not an empty string!
      res = ""
    end
    if cb then cb(ret,res) end
    return res
  end


  function _G.getpid()
    return ffi.C.getpid()
  end
  function _G.sigignore(sig)
    return ffi.C.sigignore(sig)
  end
  function _G.ignoreSIGPIPE() 
    sigignore(13) -- works for linux and osx
  end
  function _G.savePidFile(path)
    return writeFile(path, ""..getpid().."\n")
  end
  _G.SIGHUP = 1
  _G.SIGINT = 2  
  _G.SIGALRM = 14
  _G.SIGTERM = 15
  function _G.kill(pid,sig)
    return ffi.C.kill(pid,sig)
  end

  function _G.unlink(path)
    if ffi.C.unlink(path) < 0 then
      return false
    else
      return true
    end    
  end  
  _G.exit = process.exit
  if not lumino.signalSet then lumino.signalSet = {} end
  function _G.signal(sig,fn)
    ffi.C.signal(sig,fn)
    lumino.signalSet[sig] = true
  end
  
  -- signal handle tables
  -- sigtbl : { [2] => { fn1, fn2, .. }, [13] => { fn3, fn4, .. } }
  if not lumino.sigtbl then
    lumino.sigtbl = {} 
    for i=0,16 do lumino.sigtbl[i] = {} end
  end  

  function _G.addTrap(sig,fn)
    if lumino.signalSet[sig] then
      error( "don't use with signal()! sig:"..sig)
    end    
    insert( lumino.sigtbl[sig], fn )
    signal( sig, function()
        for i,f in ipairs( lumino.sigtbl[sig] ) do
          f()
        end
      end)
    lumino.signalSet[sig]=false
  end

  function _G.createCleanPidFile(path)
    if not path then
      print( "createCleanPidFile: pid file path is not set")
      return false
    end

-- signal
    addTrap( SIGTERM, function()
        print( "got sigterm. unlink:", path )
        unlink( path)
        exit(1)
      end)
    if not savePidFile(path) then
      error( "cannot save pidfile:"..path )
    end
    
    return true
  end


end

_G.extToMimeType = {
  txt = "text/plain",
  md = "text/plain",
  js = "text/javascript",
  json = "application/json",
  css = "text/css",
  png = "image/png",
  jpg = "image/jpeg",
  jpeg = "image/jpeg",
  gif = "image/gif",
  bmp = "image/bmp",  
  html = "text/html",
  htm = "text/html",
  pdf = "application/pdf",
  wav = "audio/wav",
  mp3 = "audio/mp3"
}
function _G.httpSendFile(res,path)
  local mt
  for ext,t in pairs(extToMimeType) do
    local suf = "."..ext
    local at = #path-#suf+1
    if path:find( suf,at,true) then
      mt=t
      break
    end    
  end
  if not mt then
    mt = "text/plain"
  end

  local code 
  local data = readFile(path)
  if data then
    code = 200
  else
    code = 404
    data = "not found"
  end

  if code == 200 then
    return httpSendRaw(res,code,mt,data)    
  else
    return false
  end  
end

function _G.httpSendRaw(res,code,ct,data)

  if data then
    res:writeHead( code, {
        ["Content-Type"] = ct,
        ["Content-Length"] = #data        
      })
    res:finish(data)
  else
    data = "not found"
    res:writeHead( code, {
        ["Content-Type"] = "text/html",
        ["Content-Length"] = #data
      })
    res:finish(data)    
  end
  

  return true
end

-- funcs: table with keys of function names
-- /FUNCNMAE/ARG1/ARG2/..?a=VAL&b=VAL
function _G.httpRespond(req,res,funcs)
  local url = req.url
  local qstart,qend,matched = url:find("%?(.*)$")
  if matched then
    req.query = matched
    url = url:gsub( "%?.*$","")
  else
    req.query = nil
  end
  local pathary = split(url, "/")
  remove(pathary,1) -- left token of the first /
  local fname = pathary[1]
  remove(pathary,1)
  for i,v in ipairs(pathary) do
    local n = tonumber(v)
    if n then
      pathary[i] = n
    end
  end  
  
  req.paths = pathary

  function res:sendFile(path)
    return httpSendFile(self,path)
  end
  function res:sendJSON(t)
    return httpSendRaw(self,200,"application/json",JSON.stringify(t))
  end
  function res:sendJPEG(data) httpSendRaw(self,200,"image/jpeg",data) end
  function res:sendPNG(data) httpSendRaw(self,200,"image/png",data) end
  function res:sendHTML(data) httpSendRaw(self,200,"text/html",data) end
  function res:sendError(code,body)
    return httpSendRaw(self,code,"text/html","<html><body>error code:" .. code .. "</body></html>\n")
  end


  local responder
  local responder = funcs[fname]
  if not responder then
    if funcs.default then
      responder = funcs.default
    end
    if not responder then
      print("func not found:", fname )
      return false
    end    
  end

  local expectLen = req.headers["content-length"]

  if req.method=="POST" then
    req.chunks = {}
    req:on("data",function(data)
        insert( req.chunks, data )
      end)
    req:on("end",function()
        req.body = join(req.chunks)
        responder(req,res)
      end)
    return false
  elseif req.method == "GET" then
    responder(req,res)
    return true
  end
  print("invalid method:", req.method )
  return false
end

function _G.httpServeStaticFiles(req,res,docroot,exts)
  assert(type(docroot)=="string" and #docroot>0)
  local url = req.url
  url = url:gsub("%?.*$","")

  if url:find("%.%.") then
    p("path has '..' : ", url )
    return false
  end
  local foundext = false
  if exts then
    for i,ext in ipairs(exts) do
      local suf = "." .. ext
      local at = 1 + ( #url - #suf)
      --    print( i,ext, #url, #suf, at )
      if url:find( suf,at,true ) then
        return httpSendFile(res,docroot .. url)
      end
    end
  else
    return httpSendFile(res,docroot .. url)
  end
  return false
end

if uv then
  _G.exit = process.exit
else
  _G.exit = os.exit
end


_G.pp = print

-- luvit only
if uv then
   _G.pp = luvitutils.prettyPrint
   _G.urldecode = querystring.urldecode
   _G.urlencode = querystring.urlencode
   _G.parseQueryString = querystring.parse
   
  function _G.mkdir(path)
    local err=false
    xpcall(function()
        fs.mkdirSync( path,755)
      end,
      function()
        err=true
      end)
    if err then
      return false
    else
      return true
    end
  end
  
  function _G.ensureDir(path)
    local ary = split(path,"/")
    for i=1,#ary do
      local parents = slice(ary,1,i)
      local dirp = join(parents,"/")
      mkdir(dirp) 
    end
    return existFile(path) 
  end
  
  function _G.every(t,f)
    timer.setInterval(t*1000,f)
  end
  function _G.startAdminHTTPServer(port,statfunc)
    datePrint("createAdminHTTPServer: start admin http at port:", port )
    http.createServer( function(req,res)
        local recvs = {}
        recvs.shutdown = function(req,res)
          datePrint("createAdminHTTPServer: shutdown command. killing self" )
          kill(getpid(),SIGTERM)
        end
        recvs.crash = function(req,res)
          datePrint("crash command called!")
          function_not_defined()
        end
        recvs.status = function(req,res)
          if statfunc then
            body = statfunc()
          else
            body = "ok"
          end
          res:sendHTML(body)
        end
        recvs.default = function(req,res)
          res:sendError(404)
        end
        httpRespond(req,res,recvs)
      end):listen( port, "127.0.0.1" )
  end

  -- ex:  ( "./libs", "msgpack")
  function _G.scanLuvitModule(scantop,name)
    local cwd = cmd( "pwd" ):trim()
    local s = sprintf( "find '%s/%s' -name '%s.luvit'", cwd, scantop, name )
    local fo = cmd(s)
    local ary = split(fo,"\n")
    local outmod
    for i,path in ipairs(ary) do
      path = string.gsub(path, ".luvit$","")
      if #path > 0 then 
        xpcall( function()
            outmod = require(path)
            datePrint("scanLuvitModule: module found in :",path)
          end,
          function(e)
            print(e)
          end)
        if outmod then
          return outmod
        end
      end      
    end
    return nil
  end


  -- globs: { "./*.lua", "./*/*.lua" }
  function _G.monitorFiles(globs,callback )
    local lastmtime={}
    every( 1, function()
        for _,tgt in ipairs(globs) do
          local cmd = "ls -1 " .. tgt 
          local child = spawn( "bash", { '-c', cmd }, nil ) -- { env = { TEST1 = 1 } } )
          child.stdout:on("data", function(chunk)
              local ary = split(chunk,"\n")
              for _,path in ipairs(ary) do
                if path ~= "" then
                  local s = uv.fsStat(path)
                  if not lastmtime[path] then
                    lastmtime[path] = s.mtime
                  else
                    if lastmtime[path] ~= s.mtime then
                      lastmtime[path] = s.mtime
                      callback(path)
                    end
                  end                
                end
              end
            end)
        end  
      end)
  end

  -- colors
  function _G.i32toRGB(v)
    local r,g,b = bit.rshift( bit.band( v,0xff0000 ), 16 ), bit.rshift( bit.band( v,0xff00 ), 8 ), bit.band( v,0xff )  
    return r,g,b
  end

end

  
-- MOAI funcs
function moai.now() return MOAISim.getDeviceTime() end
function moai.propDistance(p0,p1)
  local x0,y0 = p0:getLoc()
  local x1,y1 = p1:getLoc()
  return len(x0,y0,x1,y1)
end
function moai.swapPropPrio(ap,bp)
  ai = ap:getPriority()
  bi = bp:getPriority()
  ap:setPriority(bi)
  bp:setPriority(ai)
end
function moai.loadTileDeck( path, w, h, fil, wrp )
   local t = MOAITexture.new()
   t:load(path)
   if fil then t:setFilter(fil) end
   if wrp then t:setWrap(wrp) end
   local d = MOAITileDeck2D.new()
   d:setTexture(t)
   d:setSize(w,h,1/w,1/h,0,0,1/w,1/h)
   
   d:setRect( -0.5, -0.5, 0.5, 0.5 )
   return d
end

function moai.loadTileDeck2( path, w, h, sz, fullxsz, fullysz, fil, wrp )
   local t = MOAITexture.new()
   t:load(path)
   if fil then t:setFilter(fil) end
   if wrp then t:setWrap(wrp) end
   local d = MOAITileDeck2D.new()
   d:setTexture(t)
--   d:setSize(w,h,1/w,1/h,0,0,1/w,1/h)
   local xr,yr = sz/fullxsz, sz/fullysz
   d:setSize(w,h,  xr,yr, 0,0, xr,yr )
   
   d:setRect( -0.5, -0.5, 0.5, 0.5 )
   return d
 end

function moai.loadTex( path )
   local q = MOAIGfxQuad2D.new()
   q:setTexture( path )
   q:setRect( -0.5, -0.5, 0.5, 0.5 )
   return q
end

function moai.loadGfxQuad( path )
  local gq = MOAIGfxQuad2D.new()
  gq:setTexture( path )
  gq:setRect( -64,-64,64,64 )
  gq:setUVRect( 0,1,1,0)
  return gq    
end
function moai.loadImage(path)
   local img = MOAIImage.new()
   img:load( path, 0 )
   return img
end


function moai.loadWav( fn ,v)
  if not MOAIUntzSystem then
    local dummy = {}
    function dummy:play()    end
    function dummy:playDistance()    end    
    return dummy
  end
    
  local s = MOAIUntzSound.new()
  s:load( fn,true )    -- 8/15のバージョンで３つ目の引数にtrueつけると何度も再生できた
  s:setVolume(v)
  s.baseVolume = v
  s:setLooping(false)
  s:setPosition(0)
  print( "loadWav:", fn )
  function s:playDistance(x0,y0,x1,y1)
    local d = len(x0,y0,x1,y1) / ( 32 * 8 ) -- 16セルで半分
    if d < 1 then d = 1 end
    local v = self.baseVolume * 1 / d
    self:setVolume( v )
    self:play()
  end
  return s
end


-- Emulate luvit's net with luasocket. (Client only)
function moai_luvit_net_createConnection(port,ip,cb)
  print("net_createConnection called. port:",port,"ip:",ip,"cb:",cb)
  local conn = {}
  conn.sock = socket.tcp()
  conn.sock:settimeout(0)
  conn.sock:connect(ip,port)
  conn.state = "connecting"
  
  conn.sendDelay = {min=0,max=0}  -- 0.1 to simulate 100ms network send delay
  conn.sendDelayQ = {}
  conn.recvDelay = {min=0,max=0}
  conn.recvDelayQ = {}
  
  conn.callbacks = {}
  function conn:on(ev,cb)
    self.callbacks[ev] = cb
  end
  function conn:emit(data,cb)
    self:write(data,cb)
  end
  function conn:write(data,cb)
    if self.closed then
      print("write: socket closed!")
      return 0
    end
    if self.sendDelay.max > 0 then
      insert( self.sendDelayQ, data)
      local delay = range( self.sendDelay.min, self.sendDelay.max )
      self.nextSendAt = now() + delay
      return #data
    else
      return self.sock:send(data)
    end    
  end

  function conn:close()
    self.closed = true
    return self.sock:close()
  end
  
  function conn:poll()
    if self.sendDelay.max > 0 then
      if #self.sendDelayQ > 0 and now() > self.nextSendAt then
        while true do
          self.sock:send( self.sendDelayQ[1])
          remove( self.sendDelayQ, 1)
          if #self.sendDelayQ == 0 then
            break
          end
        end
      end      
    end
    if self.recvDelay.max > 0 then
      if #self.recvDelayQ > 0 and now() > self.nextRecvAt then
        if self.callbacks["data"] then
          while true do
            self.callbacks["data"]( self.recvDelayQ[1])
            remove( self.recvDelayQ, 1 )
            if #self.recvDelayQ == 0 then
              break
            end
          end          
        end
      end      
    end    
    
    if self.closed then error("socket closed") end    
    if not self.counter then self.counter = 1 end

    self.counter = self.counter + 1
    local sendt,recvt = {},{}
    table.insert(sendt,self.sock)
    table.insert(recvt,self.sock)         
    local rs,ws,e = socket.select(recvt,sendt,0)

    if ws[1] == self.sock then
      if self.state == "connecting" then
        print("luvitemu: connected!")
        if self.callbacks["complete"] then
          self.callbacks["complete"]()
        end
        self.state = "connected"
      end
    end
    if rs[1] == self.sock then
      if self.state == "connected" then
        local res, msg, partial = self.sock:receive(1024*1024*16) -- try to get everything
        if not res and msg == "closed" then
          self.state = "closed"
          if self.callbacks["end"] then
            self.callbacks["end"]()
          end                  
        else
          local got
          if partial then got = partial end
          if res then got = res end
          if got then
            if self.recvDelay.max > 0 then
              insert( self.recvDelayQ, got )
              local delay = range( self.recvDelay.min, self.recvDelay.max )
              self.nextRecvAt = now() + delay
            else
              if self.callbacks["data"] then
                self.callbacks["data"](got)
              end
            end            
          end
        end               
      end
    end
  end
  return conn
end

moai.LuvitNetEmu = {
  createConnection = moai_luvit_net_createConnection
}


-- etc/debug funcs
function _G.Counter(max)
  local t = { max = max, n = max }
  function t:get()
    if self.n > 0 then
      self.n = self.n - 1
      return true
    else
      return false
    end
  end
  function t:put()
    if self.n < self.max then
      self.n = self.n + 1
      return true
    else
      return false
    end    
  end
  return t
end

function _G.strict()
  local debug = require("debug")
  local getinfo, error, rawset, rawget = debug.getinfo, error, rawset, rawget

  local mt = getmetatable(_G)
  assert(mt==nil)
  if mt == nil then
    mt = {}
    setmetatable(_G, mt)
  end

  mt.__declared = {}

  local function what ()
    local d = getinfo(3, "S")
    return d and d.what or "C"
  end
  mt.__newindex = function (t, n, v)
    if not mt.__declared[n] then
      local w = what()
      if w ~= "main" and w ~= "C" then
        error("assign to undeclared variable '"..n.."'", 2)
      end
      mt.__declared[n] = true
    end
    rawset(t, n, v)
  end

  mt.__index = function (t, n)
    if not mt.__declared[n] and what() ~= "C" then
      error("variable '"..n.."' is not declared", 2)
    end
    return rawget(t, n)
  end
end
function _G.nostrict()
  setmetatable(_G,nil)
end

function _G.dumplocal()
  local variables = {}
  local idx = 1
  while true do
    local ln, lv = dbg.getlocal(2, idx)
    if ln ~= nil then
      if ln:byte(1,1) ~= 40 then   -- exclude '(*temporary)'  40=string.byte('(')
        table.insert( variables, { ln, lv } )
      end
    else
      break
    end
    idx = 1 + idx
  end

  local out = ""
  for i, v in ipairs(variables) do
    local x,v = v[1], v[2]
    out = out .. sprintf( "%s:%s\t", x, tostring(v) )
  end
  
  return out
end

function _G.XPMImage(w,h)
  local img = {}
  img.colors = {}
  img.pixels = {}
  img.width, img.height = w,h
  function img:addColor(r,g,b)
    local s = string.format( "#%02x%02x%02x", r,g,b)
    table.insert( self.colors, s )
    return #img.colors    
  end
  
  function img:set(x,y,colorid)
    local ix,iy = x+1, y+1
    if self.pixels[iy] == nil then
      self.pixels[iy] = {}
    end
    local row = self.pixels[iy]
    row[ix] = colorid
  end

  function img:col2str(id)
    return string.format("%02x",id)
  end
  
  function img:out(name)
    
    local outary = {}
    
    for i,v in ipairs(self.colors) do
      table.insert( outary, string.format( "\"%s c %s\",\n", self:col2str(i), self.colors[i] ) )
    end
      
    for y=0,self.height-1 do
      local row = self.pixels[y+1]
      local rowstrary = {}
      for x=0,self.width-1 do
        if not row then
          table.insert( rowstrary, "<>" )
        else
          local pixel = row[x+1]
          if pixel then
            table.insert( rowstrary, self:col2str(pixel))
          else
            table.insert( rowstrary, "<>" )
          end
        end
      end
      table.insert( outary, "\"" .. table.concat( rowstrary ) .. "\",\n" )
    end
    return  "static const char* const " .. name .. "[] = {\n" ..
      string.format( "\"%d %d %d %d\",\n", self.width, self.height, (#self.colors+1), 2 ) ..
      "\"<> c None\",\n" .. 
      table.concat( outary ) .. "};\n"
  end
  
  return img
end
  




if MOAISim then
  merge( _G, moai )
else
  merge( _G, luvit )
end

