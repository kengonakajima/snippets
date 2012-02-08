class Hoge
	def initialize(a,b,c)
		print "aho #{a} #{b} #{c}\n"
	end

end

class Hogen < Hoge

	def initialize(a,b,c)
		super
		print "hogen #{a}\n"
	end
end


a = Hogen.new("A","b","CC")
