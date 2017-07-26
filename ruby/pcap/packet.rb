require "json"

def to_hex(bary,delim)
  out=[]
  bary.each do |b| out.push( sprintf("%02x",b) ) end
  return out.join(delim)
end

class Packet
  
  def initialize(s)
    @size = s.size
    @bytes = s.bytes
  end
  def to_s
    src_mac = @bytes[0..5]
    dest_mac = @bytes[6..11]
    return JSON.generate( {
      :size => @size,
      :src_mac => to_hex(src_mac,":"),
      :dest_mac => to_hex(dest_mac,":")
      
      })
  end
  
end
