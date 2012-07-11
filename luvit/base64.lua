require("./lumino/lumino")

local base64_table = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/'
function base64(data)
  return ((data:gsub( '.', function(x)
    local r, b = '', byte(x)
    for i = 8, 1, -1 do
      r = r .. (b % 2 ^ i - b % 2 ^ (i - 1) > 0 and '1' or '0')
    end
    return r
  end) .. '0000'):gsub('%d%d%d?%d?%d?%d?', function(x)
    if #x < 6 then
      return ''
    end
    local c = 0
    for i = 1, 6 do
      c = c + (x:sub( i, i) == '1' and 2 ^ (6 - i) or 0)
    end
    return base64_table:sub( c + 1, c + 1)
  end) .. ({
    '',
    '==',
    '='
  })[#data % 3 + 1])
end

data = readFile("/etc/services")

d1 = data:sub(1,1)
d10 = data:sub(1,10)
d100 = data:sub(1,100)
d1k = data:sub(1,1000)
d10k = data:sub(1,10000)
d100k = data:sub(1,100000)

print(measure( function() enc = base64(d1); print(#enc) end ))
print(measure( function() enc = base64(d10); print(#enc) end ))
print(measure( function() enc = base64(d100); print(#enc) end ))
print(measure( function() enc = base64(d1k); print("1k:",#enc) end ))
print(measure( function() enc = base64(d10k); print("10k:",#enc) end ))
print(measure( function() enc = base64(d100k); print("100k:",#enc) end ))

