local os = require("os")
local table = require("table")
local string = require("string")

local N = 100000

local gt -- global table to store objs

function doit( caption, func1,func2)
   local t1tot, t2tot = 0,0
   local rep = 20
   gt = {}
   for i=1,rep do -- to average JIT work
      local st1 = os.clock()
      for i=1,N do
         func1(i)
      end
      local et1 = os.clock()
      t1tot = t1tot + (et1 - st1)
      local st2 = os.clock()
      for i=1,N do
         func2(i)
      end
      local et2 = os.clock()
      t2tot = t2tot + (et2 - st2)
   end

   local t1sec, t2sec = t1tot/rep, t2tot/rep
   print( string.format( "%30s creator:%.4f caller:%.4f ( %.1fM create/sec, %.1fM call/sec)", caption, t1sec, t2sec, N/t1sec/1000000, N/t2sec/1000000 ) )
end


-------------------

doit( "empty loop",
      function(i)
      end,
      function(i)
      end)
-------------------
function fixed_mul_func(x,y,v)
   return x*v, y*v
end
doit( "fixed mul func (call only)",
      function(i)
      end,
      function(i)
         fixed_mul_func(i,i,i)
      end)

-------------------

function newVector(x,y)
   local obj = {}
   obj.x = x
   obj.y = y
   obj.mul = function(self,v) return self.x*v, self.y*v end
   return obj
end
doit("init-1-meth-everytime",
     function(i)
        gt[i] = newVector(i,i)
     end,
     function(i)
        gt[i]:mul(i)
     end)
-------------------

function newVector2(x,y)
   local obj = {}
   obj.x = x
   obj.y = y
   obj.mul = function(self,v) return self.x*v, self.y*v end
   obj.mul1 = function(self,v) return self.x*v, self.y*v end
   obj.mul2 = function(self,v) return self.x*v, self.y*v end
   obj.mul3 = function(self,v) return self.x*v, self.y*v end
   obj.mul4 = function(self,v) return self.x*v, self.y*v end
   obj.mul5 = function(self,v) return self.x*v, self.y*v end
   obj.mul6 = function(self,v) return self.x*v, self.y*v end
   obj.mul7 = function(self,v) return self.x*v, self.y*v end
   obj.mul8 = function(self,v) return self.x*v, self.y*v end
   obj.mul9 = function(self,v) return self.x*v, self.y*v end
   return obj
end
doit("init-10-meth-everytime",
     function(i)
        gt[i] = newVector2(i,i)
     end,
     function(i)
        gt[i]:mul9(i,i)
     end)

-------------------
function vec_mul(self,v) return self.x*v, self.y*v end
function vec_mul1(self,v) return self.x*v, self.y*v end
function vec_mul2(self,v) return self.x*v, self.y*v end
function vec_mul3(self,v) return self.x*v, self.y*v end
function vec_mul4(self,v) return self.x*v, self.y*v end
function vec_mul5(self,v) return self.x*v, self.y*v end
function vec_mul6(self,v) return self.x*v, self.y*v end
function vec_mul7(self,v) return self.x*v, self.y*v end
function vec_mul8(self,v) return self.x*v, self.y*v end
function vec_mul9(self,v) return self.x*v, self.y*v end
function newVector3(x,y)
   local obj = {}
   obj.x = x
   obj.y = y
   obj.mul = vec_mul
   obj.mul1 = vec_mul1
   obj.mul2 = vec_mul2
   obj.mul3 = vec_mul3
   obj.mul4 = vec_mul4
   obj.mul5 = vec_mul5
   obj.mul6 = vec_mul6
   obj.mul7 = vec_mul7
   obj.mul8 = vec_mul8
   obj.mul9 = vec_mul9
   return obj
end
doit( "init-10-meth-everytime-predef",
      function(i)
         gt[i] = newVector3(i,i)
      end,
      function(i)
         gt[i]:mul9(i,i)
      end)

-------------------

Vector4 = {}
function Vector4.new(x,y)
   local obj = {}
   obj.x = x
   obj.y = y
   return setmetatable( obj,  {__index = Vector4} )
end

function Vector4.mul(self,v)
   return self.x*v, self.y*v 
end

doit( "metatable-1-meth",
      function(i)
         gt[i] = Vector4.new(i,i)
      end,
      function(i)
         gt[i]:mul(i)
      end)
         
-------------------

function Vector4.mul1(self,v) return self.x*v, self.y*v end
function Vector4.mul2(self,v) return self.x*v, self.y*v end
function Vector4.mul3(self,v) return self.x*v, self.y*v end
function Vector4.mul4(self,v) return self.x*v, self.y*v end
function Vector4.mul5(self,v) return self.x*v, self.y*v end
function Vector4.mul6(self,v) return self.x*v, self.y*v end
function Vector4.mul7(self,v) return self.x*v, self.y*v end
function Vector4.mul8(self,v) return self.x*v, self.y*v end
function Vector4.mul9(self,v) return self.x*v, self.y*v end

doit( "metatable-10-meth",
      function(i)
         gt[i] = Vector4.new(i,i)
      end,
      function(i)
         gt[i]:mul9(i,i)
      end)



function Vector4.mul10(self,v) return self.x*v, self.y*v end
function Vector4.mul11(self,v) return self.x*v, self.y*v end
function Vector4.mul12(self,v) return self.x*v, self.y*v end
function Vector4.mul13(self,v) return self.x*v, self.y*v end
function Vector4.mul14(self,v) return self.x*v, self.y*v end
function Vector4.mul15(self,v) return self.x*v, self.y*v end
function Vector4.mul16(self,v) return self.x*v, self.y*v end
function Vector4.mul17(self,v) return self.x*v, self.y*v end
function Vector4.mul18(self,v) return self.x*v, self.y*v end
function Vector4.mul19(self,v) return self.x*v, self.y*v end

doit( "metatable-20-meth",
      function(i)
         gt[i] = Vector4.new(i,i)
      end,
      function(i)
         gt[i]:mul19(i,i)
      end)


