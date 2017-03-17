def x(v,*args)
  print v, ":", args, "\n"
end

def y(v,*args)
  x(v,*args)
end
  

x(1,2,3)
y(3,4,5)