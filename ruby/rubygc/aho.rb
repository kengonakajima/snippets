
begin
	ARGV.each { |i|
		File.open( i , "w" ).write( "abcde" )
	}

rescue

end
