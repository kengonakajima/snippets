#319 318 317 316 ...         -59
# 1  2   3   4   5   6  ..     379 
# 1 2x4 4x4 6x4 8x4 (n-1)*2*4   378*2*4=
# 1 3x3 5x5 7x7 9x9

totFrame=0
totBody=0
(2..378).each do |k|
  totFrame += (k-1)*2*4
  totBody += (k*2-1)*(k*2-1)
end

print totFrame,",",totBody,"\n"
