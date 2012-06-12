module Hoge
  def define(name,val)
    define_method( name ) do |a,b| return val end 
  end
end

class A
  include Hoge
  def hoge(name,val)
    define(name,val)
  end
end

a=A.new
a.hoge( "poo", "piyo" )
