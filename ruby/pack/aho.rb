a = "�ۤ���yo"


aa = a.unpack( "c100" )

aa.each { |i|
	print "aho : #{i}\n"
}
