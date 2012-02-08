require "socket"

@sock = TCPsocket.open( "nsx", 80 )

@sock.sync = true

@sock.write( 
            "GET / HTTP/1.1\r\n" +
            "From:\r\n" +
            "Host:\r\n" +
            "\r\n" )

            
while TRUE
    out = @sock.read 
    if( out != nil ) then
        print out
    else
        break
    end
end
