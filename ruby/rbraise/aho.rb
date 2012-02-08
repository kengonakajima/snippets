
def a


begin
    raise "aho"
rescue
    print "aho: #{$!}, #{$@}\n"

end


end


a()
