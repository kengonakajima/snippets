$LOAD_PATH.push( "/opt/local/lib/ruby/site_ruby/1.8/" )
$LOAD_PATH.push( "/opt/local/lib/ruby/site_ruby/1.8/algebra" )

require "matrix-algebra"
require "mathn"

class Rational < Numeric
  def inspect; to_s; end
end
M = Algebra.MatrixAlgebra(Rational, 4, 3)
a = M.matrix{ |i,j| i*10+j}

b = a.dup
c, d, e = b.left_eliminate!
b.display #=> [1, 0, -1]
          #=> [0, 1, 2]
          #=> [0, 0, 0]
          #=> [0, 0, 0]
c.display #=> [-11/10, 1/10, 0, 0]
          #=> [1, 0, 0, 0]
          #=> [1, -2, 1, 0]
          #=> [2, -3, 0, 1]
p c*a == b#=> true
p d       #=> 1/10
p e       #=> 2
