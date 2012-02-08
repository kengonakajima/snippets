#
#
#
#


require "find.rb"

#
# path : top directory name
# fn_a : An array contains file name to be found: [ "*.html", "*.txt", "aho.c" ]
# ex_a : An array contains file name not to be found.
# You can not use regexp here : ["aho.txt" ]
#
def find_files( path, fn_a, ex_a )

	Find::find( path ) { |f|
		print f, "\n"
	}

end

find_files( "/tmp", nil,nil)
