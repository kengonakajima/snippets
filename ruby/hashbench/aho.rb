

a = Hash.new


100000.times { |i|
	a[i] = rand(768766757)
}

100000.times {
	b = a[rand(100000)]
}



