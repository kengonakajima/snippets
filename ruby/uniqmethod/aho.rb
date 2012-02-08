class Hoge
	def aho(a)
		STDERR.print a, "\n"
	end

end


a = Hoge.new

def a.bbb(a)
	STDERR.print a,a,a, "\n"
end

a.bbb(10)

