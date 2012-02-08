
require "socket"




ary = Array.new


max_task = 10

max_task.times { |i|
    ary[i] = nil
}


while TRUE
    STDERR.print "asdf\n"
    sleep 0.1
     


    max_task.times { |i|
        if( ary[i] == nil ) then
            STDERR.print "new #{i}\n"
            ary[i] = TCPSocket.open( "www.ce-lab.net", 80 )
        end
    }

    a = IO.select( ary,ary,nil,1.0 )

    if(a) then
        if(a[0])then
            a[0].each { |x|
                STDERR.print "q=#{x.to_s}\n"
                b = x.read
                if( b.nil? ) then
                    x.close
                    max_task.times { |i|
                        if( ary[i] == x ) then
                            ary[i] = nil
                        end
                    }
                else
                    STDERR.print x.read,"\n"
                end
            }
        end
        if( a[1] )then
            a[1].each { |x|
                begin
                    x.write( "GET / HTTP/1.0\r\n\r\n" )
                rescue
                end
            }
        end

    end

    
end

