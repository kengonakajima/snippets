-- OOP system from luvit
local Object = {}
Object.meta = {__index = Object}

function Object:create()
  local meta = rawget(self, "meta")
  if not meta then error("Cannot inherit from instance object") end
  return setmetatable({}, meta)
end

function Object:new(...)
  local obj = self:create()
  if type(obj.initialize) == "function" then
    obj:initialize(...)
  end
  return obj
end
function Object:extend()
  local obj = self:create()
  obj.meta = {__index = obj}
  return obj
end

-- example/benchmark follows.
local Rectangle = Object:extend()
local Square = Rectangle:extend()
local Square2 = Square:extend()
local Square3 = Square2:extend()
local Square4 = Square3:extend()
local Square5 = Square4:extend()
local Square6 = Square5:extend()

function doit(o)
  local st = os.clock()
  for i=1,100000 do
    local s = o:new(1)
  end
  local et = os.clock()
  print("t:", (et-st))
end

doit(Object)
doit(Rectangle)
doit(Square)
doit(Square2)
doit(Square3)
doit(Square4)
doit(Square5)
doit(Square6)

-- Objectから、Rectangleの関数が見えないようにしたい。
-- 今は異なるテーブルにはアクセスできない、という境界を使って制約している。

-- 上位の関数をextendするときに全部キャッシュすればいいかな？ : メンバアクセスも必要。
-- メンバ定義や、関数定義が全部専用の関数を経由する必要がある、というのは、つかいづらすぎるだろう。
-- よっていまのところ、多段metatable以外の解決策はない。


  