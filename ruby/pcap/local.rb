require "ffi/pcap"

devname = ARGV[0]
pcap = FFI::PCap::Live.new( :dev =>  devname,
                            :timeout => 1,
                            :promisc => true,
                            :handler => FFI::PCap::Handler )
pcap.nonblocking=true
pcap.setfilter("")

#print FFI::PCap.lookupnet(pcap.device)
#print pcap.netmask # => 255.255.255.0
#print pcap.network # 192.168.11.0
#print pcap.device # 192.168.11.0
#FFI::PCap.each_device do |dev|
#  if dev.name == ARGV[0] then 
#    addr = dev.addresses.addr
#    print "addr: ", addr, "\n"
#    print "  len: ", addr.len, "\n"
#    print "  family: ", addr.family, "\n"
#    print "  data: ", addr.data, "\n"
#  end
#end
# うーんわからん

`ifconfig #{devname}`.split("\n").each do |line|
   line.strip!
   tks = line.split(/\s+/)
   if tks[0] == "inet" then
     print "addr:", tks[1], "\n"
   end
end


exit 0


fd = pcap.fileno
io=IO.new(fd)
while Kernel.select([io],nil,nil)
  pcap.dispatch() do |this,pkt|
    p = Packet.new( pkt.body)  # pkt.body.each_byte {|x| STDERR.print "%0.2x " % x }
    pn = p.get(:ipv4_protocol)
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

    
