luabit = require "./luabit"

function from_double(x)
  local function grab_byte(v)
    return math.floor(v / 256),
           string.char(math.mod(math.floor(v), 256))
  end
  local sign = 0
  if x < 0 then sign = 1; x = -x end
  local mantissa, exponent = math.frexp(x)
  if x == 0 then -- zero
    mantissa, exponent = 0, 0
  elseif x == 1/0 then
    mantissa, exponent = 0, 2047
  else
    mantissa = (mantissa * 2 - 1) * math.ldexp(0.5, 53)
    exponent = exponent + 1022
  end
  local v, byte = "" -- convert to bytes
  x = mantissa
  for i = 1,6 do
    x, byte = grab_byte(x); v = v..byte -- 47:0
  end
  x, byte = grab_byte(exponent * 16 + x);  v = v..byte -- 55:48
  x, byte = grab_byte(sign * 128 + x); v = v..byte -- 63:56
  return v
end


function bittodouble(ary)
   local x = 0
   local cur = 0.5
   for i,v in ipairs(ary) do
      x = x + cur * v
      cur = cur / 2
   end
   return x   
end

function bytetobits(ary)
   local out={}
   for i,v in ipairs(ary) do
      for j=0,7,1 do
         table.insert(out, luabit.band( luabit.brshift(v,7-j), 1 ) )
      end
   end
   return out
end

function dumpbits(ary)
   local s=""
   for i,v in ipairs(ary) do
      s = s .. v .. " "
      if (i%8)==0 then s = s .. " " end
   end
   print(s)
end

function to_double(v)
   -- sign:1bit
   -- exp: 11bit (2048, bias=1023)
   local sign = math.floor(v:byte(8) / 128)
   local exp = luabit.band( v:byte(8), 127 ) * 16 + luabit.brshift( v:byte(7), 4 ) - 1023 -- bias
   -- frac: 52 bit
   local fracbytes = {
      luabit.band( v:byte(7), 15 ), v:byte(6), v:byte(5), v:byte(4), v:byte(3), v:byte(2), v:byte(1) -- big endian
   }
   local bits = bytetobits(fracbytes)
   
   for i=1,4 do table.remove(bits,1) end

--   dumpbits(bits)

   if sign == 1 then sign = -1 else sign = 1 end
   
   local frac = bittodouble(bits)
   if exp == -1023 and frac==0 then return 0 end
   if exp == 1024 and frac==0 then return 1/0 *sign end
   local real = math.ldexp(1+frac,exp)

--   print( "sign:", sign, "exp:", exp,  "frac:", frac, "real:", real )
   return real * sign
end

function v2str(v)
   local s = ""
   for i=1,#v do
      s = s .. string.format( "%x,", v:byte(i,i) )
   end
   return s
end


function doit(orig)
   local v = from_double(orig)
   local vv = to_double(v)
--   print( orig, v2str(v),  vv )
   if orig ~= vv then error("not match") end
end

doit(0.1)
doit(0.01)
doit(0.001)
doit(0.0001)
doit(1.1)
doit(-1)
doit(1)
doit(0)
doit(-0)
doit(1/0)
doit(-1/0)
doit(1.2345e-10)

for i=1,10000 do
   local n = math.ldexp( 10, 300 * math.random() )
   doit(n)
end
