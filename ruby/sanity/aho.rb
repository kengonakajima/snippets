
def t(x)
    a = x.split("")
    a.each { |i|
        next if( i =~ /[a-zA-Z0-9]/ ) 
        return false
    }
    return true
end

a = "asd84i9s"

print t(a) , "\n"

a = "ahoa(adf)--"

print t(a) , "\n"


