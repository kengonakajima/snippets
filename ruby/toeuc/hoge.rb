aho = [ "なすもの" , "はだれも" , "いないのだ" ]

require "kconv"


print Kconv.tosjis( aho ).join(";") , "\n"
