require "socket"

gs = TCPServer.open(9999)
addr = gs.addr
addr.shift
printf("server is on %s\n", addr.join(":"))

while true
  Thread.start(gs.accept) do |s|       # save to dynamic variable
    print(s, " is accepted\n")
    while s.gets
      s.write($_)
	  print "data:" , $_, "\n"
    end
    print(s, " is gone\n")
    s.close
  end
end
