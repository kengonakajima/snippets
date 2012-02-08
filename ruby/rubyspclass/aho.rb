a = Object.new
b = Object.new

class << a

	def hoge
		print "aho\n"
	end
end


a.hoge
b.hoge
