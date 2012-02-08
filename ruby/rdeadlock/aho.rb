require "socket"
require "thread"

gs = TCPserver.open( 4444 )


while TRUE
	ns = gs.accept
	STDERR.print ns, " is accepted\n"
	Thread.start do
		s = ns
		while TRUE
			sleep 0.1
			a = s.read
			STDERR.print a, " is read from socket\n"
		end
		STDERR.print s, " is gone\n"
		s.close
	end
end
