require "socket"
s = TCPSocket.open("www.ce-lab.net",80)
 s.write("GET / HTTP/1.0\r\n\r\n")

while l=s.gets
  print(l)
end
s.close
