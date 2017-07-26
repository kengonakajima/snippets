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
    type = @bytes[12..13]

    out_h = {
      :size => @size,
      :src_mac => to_hex(src_mac,":"),
      :dest_mac => to_hex(dest_mac,":"),
      :type => type
    }

    if type[0]==8 and type[1] == 0 then # ipv4
      out_h[:ipv4_version] = (@bytes[14] >> 4) & 0xf
      ipv4_header_len = @bytes[14] & 0xf
      out_h[:ipv4_header_len] = ipv4_header_len
      out_h[:ipv4_service_type] = @bytes[15]
      ipv4_total_len = @bytes[16]*256 + @bytes[17]
      out_h[:ipv4_total_len] = ipv4_total_len
      out_h[:ipv4_identifier] = to_hex( @bytes[18..19], ":")
      out_h[:ipv4_flag] = (@bytes[20] & 0xff)>>5 
      out_h[:ipv4_fragpos] =  (@bytes[20] & 0x1f)*256 + @bytes[21]
      out_h[:ipv4_ttl] = @bytes[22]
      out_h[:ipv4_protocol] = @bytes[23] # 6 for TCP, 17 for UDP
      out_h[:ipv4_checksum] = to_hex( @bytes[24..25],":")
      out_h[:ipv4_src_addr] = @bytes[26..29].join(".")
      out_h[:ipv4_dest_addr] = @bytes[30..33].join(".")
      # ignoring ipv4 extention field
      ipv4_payload_len = ipv4_total_len - ipv4_header_len*4
      out_h[:ipv4_payload_len] = ipv4_payload_len


      
    end
    return JSON.generate(out_h)
  end
  
end
