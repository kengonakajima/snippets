a = "¤Û¤²¤Àyo"


aa = a.unpack( "c100" )

aa.each { |i|
	print "aho : #{i}\n"
}
