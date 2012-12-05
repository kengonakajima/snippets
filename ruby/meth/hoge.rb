class X
  def >>(x)
    print( ">> #{x*2}")
  end
  def initialize()
    >>(8)
  end
end

x = X.new

