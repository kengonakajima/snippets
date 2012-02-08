aho = [ "<func>" , "<ououo>", "<nice>" , "<catego>" ]

aho.each { |i|
	if( i =~ /^<(func|nice)>/ ) then
		print "K: #{$1}\n"
	end
}
