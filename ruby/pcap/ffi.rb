require "ffi/pcap"

if !ARGV[0] then 
  STDERR.print "need intf name\n"
  exit 1
end

pcap = FFI::PCap::Live.new( :dev =>  ARGV[0],
                            :timeout => 1,
                            :promisc => true,
                            :handler => FFI::PCap::Handler )

pcap.setfilter("tcp")

pcap.loop() do |this, pkt|
  STDERR.puts "#{pkt.time} : "
  pkt.body.each_byte {|x| STDERR.print "%0.2x " % x }
  STDERR.putc "\n"
end



trap :INT do 
  exit 0
end
trap :TERM do 
  exit 0
end
