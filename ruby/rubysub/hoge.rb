def f1(a)
	a+=1
	return a if( a >10)
	print "f1:" , f2(a) , "\n"
	return a
end

def f2(a)
	a+=1
	return a if(a>10)
	f1(a)
	return a
end

f1(1)
