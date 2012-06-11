def argdump(b)
  out = b.eval( <<EOF
__s = ""
local_variables.each do |name| 
  if name != "__s" then
    __s += name + ":"
    __s += eval( name + ".to_s" )
    __s += '\t'
  end
end
__s += "\n"
EOF
)
  return out
end

def hoge(a,b,c)
  print local_variables.join(","),"\n"
  local_variables.each do |name|
    puts eval name
  end
end

def piyo(a,b,c)
  print( argdump(binding) )
end


hoge(1,2,3)

piyo(4,5,6)
