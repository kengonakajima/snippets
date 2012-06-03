A={}
function A:new()
  local o = {}
  setmetatable(o,self)
  self.__index = self
  return o
end

a = A:new()
aa = A:new()
print(a)
print(aa)

----


t1 = {a=3}
t2 = {}

setmetatable(t1,t2)
t3 = { c=5 }

t2.__index = t3

print(t1.a)
print(t1.b)
print(t1.c)

_G.string = require("string")

s="aho"
print(string.byte(s,1,1))

print( s:byte(1,1))
t={1,2,3}

_G.table = require("table")
hoge = {}
function hoge:copy()
  return {}
end

setmetatable(t,hoge)
hoge.__index=hoge
tt = t:copy()

ex = {aa=function(e) print(e) end}
setmetatable(table,ex)
ex.__index=ex

print( getmetatable(table) )


string.aa = function(e) print(e) end
s="aho"
s:aa()

table.aa = function(e) print(e) end
hoge={1,2,3,4}
hoge:aa()