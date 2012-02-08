$LOAD_PATH.push( "/opt/local/lib/ruby/site_ruby/1.8/" )
$LOAD_PATH.push( "/opt/local/lib/ruby/site_ruby/1.8/algebra" )

require "matrix-algebra"
require "mathn"
M = Algebra.MatrixAlgebra(Rational, 5, 4)
a = M.matrix{|i, j| 
	print "ij:" , i , ",", j, "\n"
	i+j
}
a.set_row(0, [9,9,9,9])
a[0,0]=10

a.display
a.kernel_basis.each do |v|
	puts "a * #{v} = #{a * v}"
end
