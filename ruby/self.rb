class A
  def aho(a,&b)
    b.call(a)
  end
  def q()
    return "QQ"
  end
end
a = A.new()
a.aho(1) do |a|
  print "aa:",a," ", self, "\n"
  print self.q
end
