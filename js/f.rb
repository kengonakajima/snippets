def fff(x)
  x+2
end
class A
  attr_accessor :p  
  def initialize(x)
    @p = x
  end
end
def ggg(x)
  a = A.new(x)
  a.p
end

t=0
100000000.times do |i|
  t += ggg(i)
end

print "ttt:#{t}"
