class Cli
	def hoge
		return 2
	end
end

a = Cli.new

a.hoge.times{ |i|
	print "aho"
}
