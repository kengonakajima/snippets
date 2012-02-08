class Client
	def initialize(a)
		@hoge = a
	end
	def hoge
		return @hoge
	end
end



a = Client.new(1)
b = Client.new(2)

print a.hoge
print b.hoge
