b = Proc.new {
	break
}
a = Proc.new {
	3
	break
}


print a.call, "\n"
print b.call, "\n"

