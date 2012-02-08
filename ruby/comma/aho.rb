

def conv3(bign)
    
    str = bign.to_s
    n = str.length

    out = ""
    n.times { |k|
        index = n - 1 - k
        s = sprintf ("%c", str[index] )

        out += s
        
        if ( ( k % 3 ) == 2 and index != 0 ) then
            out += ","
        end        
    }

    return out.reverse

end



print conv3(84409384059345),"\n"
print conv3(184409384059345),"\n"
print conv3(2184409384059345),"\n"
print conv3(32184409384059345),"\n"
print conv3(432184409384059345),"\n"
	


