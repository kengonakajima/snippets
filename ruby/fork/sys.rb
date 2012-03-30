cnt=0
while true do
    sleep 1
    print "aho:#{cnt+=1}\n"
    system( "ruby -e 'sleep 2;print #{cnt}' &" )
end
