require( "./lumino/lumino")

local n = 100*1000*1000

function shortname()
  local tot = 0
  for i=1,n do
    local t = {
      a=1,b=2,c=3,d=4,e=5,f=6,g=7
    }
    tot = tot + t.a + t.b + t.c + t.d + t.e + t.f + t.g
  end
  print(tot)
end

function longname()
  local tot = 0
  for i=1,n do
    local t = {
      aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa=1,
      bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb=2,
      cccccccccccccccccccccccccccccccccccccccccccccccc=3,
      dddddddddddddddddddddddddddddddddddddddddddddddd=4,
      eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee=5,
      ffffffffffffffffffffffffffffffffffffffffffffffff=6,
      gggggggggggggggggggggggggggggggggggggggggggggggg=7
    }
    tot = tot + t.aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
    + t.bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb + t.cccccccccccccccccccccccccccccccccccccccccccccccc
    + t.dddddddddddddddddddddddddddddddddddddddddddddddd + t.eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee
    + t.ffffffffffffffffffffffffffffffffffffffffffffffff + t.gggggggggggggggggggggggggggggggggggggggggggggggg
  end
  print(tot)
end

function strname()
  local tot = 0
  for i=1,n do
    local t = {
      aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa=1,
      bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb=2,
      cccccccccccccccccccccccccccccccccccccccccccccccc=3,
      dddddddddddddddddddddddddddddddddddddddddddddddd=4,
      eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee=5,
      ffffffffffffffffffffffffffffffffffffffffffffffff=6,
      gggggggggggggggggggggggggggggggggggggggggggggggg=7
    }
    tot = tot + t["aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"]
      + t["bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"] + t["cccccccccccccccccccccccccccccccccccccccccccccccc"]
        + t["dddddddddddddddddddddddddddddddddddddddddddddddd"] + t["eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee"]
          + t["ffffffffffffffffffffffffffffffffffffffffffffffff"] + t["gggggggggggggggggggggggggggggggggggggggggggggggg"]
  end
  print(tot)
end

t = measure(function() shortname() end)
print(t)


t = measure(function() longname() end)
print(t)

t = measure(function() strname() end)
print(t)
