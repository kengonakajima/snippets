
def t(x)
    ary = x.split("")
    return false if( ary.size < 4 or ary.size > 12 ) 
    lc = ary[ary.size-1]
    fc = ary[0]
    if( fc == "[" or fc == "=" or fc == "-" ) then
        if ( fc == "[" and lc != "]" ) then
            return false 
        end
        if( fc == "=" and lc != "=" ) then
            return false
        end
        if( fc == "-" and lc != "-" ) then
            return false
        end
        return true
    else
        return false
    end
end

a = "[ABC]"

print t(a) , "\n"
a = "-ABC"
print t(a) , "\n"
a = "[あほ]"
print t(a) , "\n"
a = "ほげ="
print t(a) , "\n"

a = "[adsfasdfasdfasdfasdf]"
print t(a) , "\n"

a = "[f]"
print t(a) , "\n"
