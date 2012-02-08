def numofbits(bits)
		bits = (bits & 0x55555555) + (bits >> 1 & 0x55555555);
		bits = (bits & 0x33333333) + (bits >> 2 & 0x33333333);
		bits = (bits & 0x0f0f0f0f) + (bits >> 4 & 0x0f0f0f0f);
		bits = (bits & 0x00ff00ff) + (bits >> 8 & 0x00ff00ff);
		return (bits & 0x0000ffff) + (bits >>16 & 0x0000ffff);
end

st = [ "C", "G", "A", "T" ]

n = ARGV[0].to_i
for i in (0 .. n-1 ) 
	num = numofbits(i)
	print( st[num%4] );
end

