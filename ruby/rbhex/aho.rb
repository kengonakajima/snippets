a = "0x03"


print eval( "0x12" ) , "\n"

def hexto_i( str )
  str =~ /0x([0-9]+)/
  i = $1.unpack( "h*")
  i.each { |aho|
    print "A:#{aho}:\n"
  }
end

print hexto_i(a) , "\n"
