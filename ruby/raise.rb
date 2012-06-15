
class A < Exception
  def initialize(x)
    @x = x
  end
  def to_s()
    return @x
  end
end

begin
  raise A.new("aaaaaa")
rescue 
  print( "aa:", $!, ",", $!.class)
end
