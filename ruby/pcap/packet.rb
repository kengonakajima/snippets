require "json"

class Packet
  
  def initialize(s)
    @size = s.size
  end
  def to_s
    return JSON.generate( { :size => @size } )
  end
  
end
