class A
  def initialize()
    @hoge = 0
    @h = {}
  end
  def []= (x,y)
    @h[x] = y
  end
  def [] (x)
    return @h[x]
  end
  attr_accessor "poo"
  def method_missing(name,*args)
    print("MM:", name )
  end
end

a=A.new

a[1] = 2

a.poo = 2

print( "hoge:", a[1], "\n" )
print( "p:", a.poo )

a.poka = 4
