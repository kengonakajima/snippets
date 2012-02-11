a = "hogeほげ\0 abc\n\r\b\t\\\'\"\x1aABC"
print(a)
print(#a)
for i=1,#a do
  print( string.byte(a,i) )
end

local repl = {
  [string.char(0)] = "\\0",
  ['\n'] = "\\n",
  ['\r'] = "\\r",
  ['\b'] = "\\b",
  ['\t'] = "\\t",
  ['\x1a'] = "\\Z",
  ['\\'] = "\\\\"
}
--local b = string.gsub( a, "[\0\n\r\b\t\\\'\"\x1a]", repl )
local b = string.gsub( a, "["..string.char(0).."]", repl ) -- pattern is terminated by zero byte, and ends in syntax error.
print(b)