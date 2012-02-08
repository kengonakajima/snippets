def fib n
	a = 0
	b = 1
	n.times { |i|
		temp = a + b
		a = b
		b = temp
	}
	a
end


print fib(300000),"\n"

