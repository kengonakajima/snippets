class Aho
	def initialize
		print "Aho\n"
	end

end

class Hoge
	def initialize
		print "Hoge\n"
	end
	def nasu
		print "nasu"
	end
end


a = "$aho = Hoge.new"


eval(a)

$aho.nasu

