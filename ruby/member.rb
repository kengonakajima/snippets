class A
  attr_accessor :m

  def x
    return m
  end
end


a = A.new()

print a.m, "\n"
a.m=1
print a.m, "\n"

print a.x(), "\n"
