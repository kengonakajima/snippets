
a = [ "asdf" , "hoge", "moge" ]

a.filter { |i|
	i += "K"
}

print a.join(";")
