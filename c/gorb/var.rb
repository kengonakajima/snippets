#
# varの実験
#

def int(*args)
  return "int"
end

def var(*args)
  print "var:", args[0],"\n"
end

def method_missing(n,*args)
  print "name:",n," arg0:#{args[0]}\n"
    return n
end




var i int


var ia int[0]
