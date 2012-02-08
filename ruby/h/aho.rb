    require "cgi"





def main
    h = [ 
        { "d" => 123, "e" => "asdfasdf" } ,
        { "d" => 34123, "e" => "asdfasdasdf<  a>sdfasdff" } 
    ]

    a = {
        "a" => "a<d>f",
        "b" => 234,
        "c" => h
    }

    p a 

    aa = conv(a)
    
    p aa
end


def conv(x)
    if( x.type.to_s == "Hash" ) then
        print "@@@ Hash\n"
        ra = Hash.new
        x.keys.each { |k|
            if( x[k].type.to_s == "String" ) then
                ra[k] = CGI::escape(x[k])
            elsif( x[k].type.to_s == "Array" or x[k].type.to_s == "Hash" ) then
                ra[k] = conv(x[k])
            else
                ra[k] = x[k]
            end
        }
        return ra
    elsif( x.type.to_s == "Array" ) then
        print "@@@ Array\n"
        ra = Array.new
        x.each { |i|
            if( i.type.to_s == "String" ) then
                ra.push(CGI::escape(x[i]))
            elsif( i.type.to_s == "Array" or i.type.to_s == "Hash" ) then
                ra.push( conv(i))
            else
                ra.push(i)
            end
        }
        return ra
    else
        print "@@@ Other(#{x.type.to_s})\n"
    end
end

main()
