require "socket"

gs = TCPServer.open(9999)
addr = gs.addr
addr.shift
printf("server is on %s\n", addr.join(":"))

while true
  Thread.start(gs.accept) do |s|       # save to dynamic variable
    print(s, " is accepted\n")
    buf = ""
    while x = s.getc
      buf.concat(x)
      if buf == "<policy-file-request/>" then
        s.write( "<?xml version=\"1.0\"?><cross-domain-policy>  <allow-access-from domain=\"*\" to-ports=\"*\" secure=\"false\" /></cross-domain-policy>" )
        s.flush
      else
		if buf[0..0] != "<" then
          s.write( "".concat(x))
          s.flush
        end
      end
#      s.write(x)
	  STDERR.print "".concat(x)
    end
    print(s, " is gone\n")
    s.close
  end
end