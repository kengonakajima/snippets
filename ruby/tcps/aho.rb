require "socket"
require "thread"

gs = TCPServer.open( "0.0.0.0", 80 )

while TRUE
	ns = gs.accept
	STDERR.print ns, " is accepted\n"

    while TRUE
        sleep 0.1
        a = ns.sysread(10)
        STDERR.print a, " is read from socket\n"
    end
    ns.close

end

