def method_missing(n,*args)
  print "name:",n," arg0:#{args[0]}\n"
    return n
end


ahoahoa(1,"2");

