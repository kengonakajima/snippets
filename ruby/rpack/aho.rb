require "socket"
s = "210.239.244.116"
sa = []
s.split(/\./).each{ |i|
	sa.push(i.to_i)
}
ip = sa.pack("CCCC")
hn = Socket.gethostbyaddr( ip )[0]

print hn


