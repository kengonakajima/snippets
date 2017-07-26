require "ffi/pcap"
require "json"


def to_hex(bary,delim)
  out=[]
  bary.each do |b| out.push( sprintf("%02x",b) ) end
  return out.join(delim)
end
def to_i32(bary)
  return (bary[0]<<24) + (bary[1]<<16) + (bary[2]<<8) + bary[3]
end


class Packet
  
  def initialize(s)
    @size = s.size
    @bytes = s.bytes
    @h = to_hash()
  end
  def to_hash
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
      ipv4_protocol = @bytes[23] # 6 for TCP, 17 for UDP
      out_h[:ipv4_protocol] = ipv4_protocol
      out_h[:ipv4_checksum] = to_hex( @bytes[24..25],":")
      out_h[:ipv4_src_addr] = @bytes[26..29].join(".")
      out_h[:ipv4_dest_addr] = @bytes[30..33].join(".")
      # ignoring ipv4 extention field
      ipv4_payload_len = ipv4_total_len - ipv4_header_len*4
      out_h[:ipv4_payload_len] = ipv4_payload_len

      if ipv4_protocol == 6 then
        tcptop = 14+ipv4_header_len*4  # mostly 14+5*4=34
        out_h[:tcp_src_port] = @bytes[tcptop]*256 + @bytes[tcptop+1]
        out_h[:tcp_dest_port] = @bytes[tcptop+2]*256 + @bytes[tcptop+3]
        out_h[:tcp_seqnum] = to_i32( @bytes[(tcptop+4)..(tcptop+7)] )
        out_h[:tcp_ack_num] = to_i32( @bytes[(tcptop+8)..(tcptop+11)] )
        tcp_data_ofs = (@bytes[tcptop+12]&0xff)>>4
        out_h[:tcp_data_ofs] = tcp_data_ofs
        out_h[:tcp_flag_ns] = (@bytes[tcptop+12]&0x1)
        out_h[:tcp_flag_cwr] = (@bytes[tcptop+13]>>7)
        out_h[:tcp_flag_ece] = (@bytes[tcptop+13]>>6)&0x1
        out_h[:tcp_flag_urg] = (@bytes[tcptop+13]>>5)&0x1
        out_h[:tcp_flag_ack] = (@bytes[tcptop+13]>>4)&0x1
        out_h[:tcp_flag_psh] = (@bytes[tcptop+13]>>3)&0x1
        out_h[:tcp_flag_rst] = (@bytes[tcptop+13]>>2)&0x1
        out_h[:tcp_flag_syn] = (@bytes[tcptop+13]>>1)&0x1
        out_h[:tcp_flag_fin] = @bytes[tcptop+13]&0x1
        out_h[:tcp_window_size] = @bytes[tcptop+14]*256 + @bytes[tcptop+15]
        out_h[:tcp_checksum] = to_hex( @bytes[(tcptop+16)..(tcptop+17)], ":")
        out_h[:tcp_urg_pointer] = @bytes[tcptop+18]*256 + @bytes[tcptop+19]
        tcp_payload_len = ipv4_payload_len - tcp_data_ofs*4
        out_h[:tcp_payload_len] = tcp_payload_len
        out_h[:tcp_payload] = @bytes[(tcptop+tcp_data_ofs*4)..-1]
        if tcp_payload_len != out_h[:tcp_payload].size then
          STDERR.print "warning: tcp payload size mismatch? #{ipv4_payload_len}, #{tcp_payload_len}, #{out_h[:tcp_payload].size}\n"
        end
      elsif ipv4_protocol == 17 then
        udptop = 14+ipv4_header_len*4
        out_h[:udp_src_port] = @bytes[udptop]*256 + @bytes[udptop+1]
        out_h[:udp_dest_port] = @bytes[udptop+2]*256 + @bytes[udptop+3]
        out_h[:udp_total_data_len] = @bytes[udptop+4]*256 + @bytes[udptop+5] # including header and payload
        out_h[:udp_checksum] = to_hex( @bytes[(udptop+6)..(udptop+7)], ":")
        out_h[:udp_payload] = @bytes[(udptop+8)..-1]
        udp_payload_len = ipv4_payload_len - 8
        if udp_payload_len != out_h[:udp_payload].size then
          STDERR.print "warning: udp payload size mismatch? #{ipv4_payload_len}, #{udp_payload_len}, #{out_h[:udp_payload].size}\n"
        end
      end
    end
    return out_h
  end
  def get_protonum()
    return @h[:ipv4_protocol]
  end
  def to_id_str()
    sp = (@h[:udp_src_port] or @h[:tcp_src_port])
    dp = (@h[:udp_dest_port] or @h[:tcp_dest_port])
    return "#{@h[:ipv4_protocol]}:#{@h[:ipv4_src_addr]}:#{@h[:ipv4_dest_addr]}:#{sp}:#{dp}"
  end  
end



class Session
  def initialize(pkt)
    @idstr = pkt.to_id_str
    @packet_count=0
  end
  def same_session?(pkt)
    s = pkt.to_id_istr
    return (@idstr == s)
  end
end

$sessions={}  

def updateSessions(pkt)
  idstr = pkt.to_id_str
  s = $sessions[idstr]
  if !s then 
    s = Session.new(pkt)
    $sessions[idstr]=s
    STDERR.print "new session for #{idstr} count:#{$sessions.size}\n"
  end
end

######################


# main

if !ARGV[0] then 
  STDERR.print "need intf name\n"
  exit 1
end

pcap = FFI::PCap::Live.new( :dev =>  ARGV[0],
                            :timeout => 1,
                            :promisc => true,
                            :handler => FFI::PCap::Handler )
pcap.nonblocking=true
pcap.setfilter("")



fd = pcap.fileno
io=IO.new(fd)
while Kernel.select([io],nil,nil)
  pcap.dispatch() do |this,pkt|
    p = Packet.new( pkt.body)  # pkt.body.each_byte {|x| STDERR.print "%0.2x " % x }
    pn = p.get_protonum
    if pn == 6 or pn == 17 then 
      updateSessions(p)
    end
  end
end



trap :INT do 
  exit 0
end
trap :TERM do 
  exit 0
end
