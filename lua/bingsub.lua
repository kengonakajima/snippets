local a = "hogeほげ\0 abc\n\r\b\t\\\'\"" .. string.char(0x1a) .. "ABC"

local escapeStringRepl = {
  [0] = "\\0",
  [8] = "\\b",
  [9] = "\\t",  
  [10] = "\\n",
  [13] = "\\r",
  [26] = "\\Z",
  [34] = "\\\"",
  [39] = "\\\'",
  [92] = "\\\\"
}

function escapeString(str)
  local out = {}
  for i=1,#str do
    local byte = string.byte( str, i )
    local to = escapeStringRepl[byte]
    if to then
      table.insert( out, to )
    else
      table.insert( out, string.char(byte) )
    end
  end
  return table.concat( out )
end

print(a)
print( escapeString(a) )
