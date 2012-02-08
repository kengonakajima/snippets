require "kconv"

def kconv_ex( x )
    out = ""
    x.split("\n").each { |line|
        if( Kconv.guess(line) != Kconv::SJIS ) then
            out += Kconv.tosjis( line )
        else
            out += line
        end
    }
    return out
end


f = File.open( "get2ch.txt","r")

#
a = f.read


print kconv_ex(a)
