a = system( "./a.out" )
print "result:", a, "\n"
print "exited:", $?.exited?, "\n"
print "i:", $?.to_i, "\n"

b = system( "ls" )
print "result:", b, "\n"
print "exited:", $?.exited?, "\n"
print "i:", $?.to_i, "\n"