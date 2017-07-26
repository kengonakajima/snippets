require "ffi/pcap"

pcap = FFI::PCap::Live.new( :dev =>  "en0",
                            :timeout => 1,
                            :promisc => true,
                            :handler => FFI::PCap::Handler )

pcap.setfilter("tcp")

pcap.loop() do |this, pkt|
  puts "#{pkt.time} : "
  pkt.body.each_byte {|x| print "%0.2x " % x }
  putc "\n"
end



trap :INT do 
  exit 0
end
trap :TERM do 
  exit 0
end
