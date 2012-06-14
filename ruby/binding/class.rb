def func(b)
  print "func:", @mo,"\n"
end


class A
  def hoge()
    @ma = 111
  end
  def fuga()
    func(binding)
    print "ma:",@ma,"\n"
  end
end


a=A.new
a.hoge()
a.fuga()
