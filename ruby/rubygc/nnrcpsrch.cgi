#!/usr/local/bin/ruby

begin
	require "phrase_tools.rb"
	eval( File.open( "host_config.rb" ).read )
	require "MySQL_NNRC_Query_Generator.rb"
	require "cgi"

	cgi	= CGI.new
	gen	= MyNNRCQGENE.new(HOST, USER, PWD, DB)

	str	= cgi['phrase'][0]
	unless nilnul(str)	then
		str	= chg_phrase(str)
		gen.set_phrase(str)
	end

	ret	= gen.search
	print <<-EOH
Content-type: text/html

<html>
<body>
	EOH
	i	= 0
	ret.each do |r|
		i	+= 1
		print r['id']
		print "\n"
		
		aaa = File.open( "tmp_#{i}.bmp","w" )
#		GC.start
print aaa.write(r['data'])
aaa.close
		print "<img src='tmp_#{i}.bmp'>"
		print "\n"
		print r['description']
		print "\n"
		print "<br>"
		print "\n"
	end

	print <<-EOH
<hr>
<center>
<a href="./menu.html">back.</a>
</center>
</body>
</html>
	EOH

rescue

	print <<-EOH
Content-type: text/html

<html>
<body>
<h1>
Error.
</h1>
#{$!}
<hr>
<center>
<a href="./menu.html">back.</a>
</center>
</body>
</html>
	EOH

end
