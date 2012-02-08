
[ "hoge" , "moge" ].each{ |i|

	OUT = File.open( i, "w" )
	OUT.printf( "hogehoge\n" )
	OUT.close
	OUT.finalize
}
