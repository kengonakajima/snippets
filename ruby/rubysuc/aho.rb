class A
	
	def hoge
		print defined? aaa
	end
end

class Aaa < A
	def aaa
	end
end


h = Aaa.new

h.hoge
