def method_missing(n,*args)
  print "name:",n," arg0:#{args[0]}\n"
    return n
end


ahoahoa(1,"2");


a = "hoge"

def a.method_missing(n,*args)
  print "AAAAAAAAA #{n} #{args}\n"
end


a.notdefined(1,2,3)
