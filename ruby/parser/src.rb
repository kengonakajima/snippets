class X
  def mb1(p)
  end
  def mb2(p)
  end
end

def long(a,b,c,d,e,f,g,h)
end

def hoge(a,b)
  print a,b
  x=X.new
  x.mb(1)
end

def conditional1(arg1)
  if arg1 == 0 then
    return 1
  end
  hoge(1,2,3)
  return 0
end
