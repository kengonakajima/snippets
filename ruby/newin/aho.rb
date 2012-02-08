def newind()

	a  = Dir.glob( "_aho_*" )
	print a.join( ":" ) , "\n"
	sorted = a.sort { |a,b|
		print "A,B: #{a} , #{b}\n"
		a =~ /_aho_(.+)/ 
		ai = $1
		b =~ /_aho_(.+)/
		bi = $1

		if( ai < bi ) then
			return -1
		end
		if( ai > bi ) then
			return 1
		end
	}
	
	print sorted.join( ":" ) , "\n"

end


newind()
