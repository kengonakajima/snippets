
a = "ringo@hompo.co.jp,hio@hompo.co.jp"

a.split( /,/ ).each{ |i|
	print "#{i}\n"
}
