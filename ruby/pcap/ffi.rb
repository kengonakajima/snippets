require "ffi/pcap"

require "./packet.rb"

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
    h = p.to_hash
    if h[:ipv4_protocol]==17 then
      STDERR.puts "#{pkt.time} : "
      STDERR.print p.to_s(true)
    end
    
#    STDERR.print pkt.header.methods.sort, "\n"
  end
end



trap :INT do 
  exit 0
end
trap :TERM do 
  exit 0
end
