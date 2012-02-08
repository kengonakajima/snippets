$LOAD_PATH.push( "/opt/local/lib/ruby/site_ruby/1.8/" )
$LOAD_PATH.push( "/opt/local/lib/ruby/site_ruby/1.8/algebra" )

x1 = [ 1.2, 1.6, 3.5, 4.0, 5.6, 5.7, 6.7, 7.5, 8.5, 9.7 ]
x2 = [ 1.9, 2.7, 3.7, 3.1, 3.5, 7.5, 1.2, 3.7, 0.6, 5.1 ]
y  = [ 0.9, 1.3, 2.0, 1.8, 2.2, 3.5, 1.9, 2.7, 2.1, 3.6 ]

#
class Array
    def avg
        t = 0.0
        self.each { |x| t += x }
        return t / self.size
    end
end

print x1.avg, "\n"
print x2.avg, "\n"
print y.avg, "\n"

def s(i,j)
	t = 0.0
    n = i.size
    iavg = i.avg
	javg = j.avg
    n.times { |index|
		t += ( i[index] - iavg ) * ( j[index] - javg )
    }
	return t
end

print s(x1,x1) , "\n"
print s(x1,x2) , "\n"
print s(x2,x1) , "\n"
print s(x2,x2) , "\n"
print s(x1,y), "\n"
print s(x2,y), "\n"

require "matrix-algebra"
require "mathn"

m = Algebra.MatrixAlgebra(Rational, 2,3 )
a = m.matrix { |i,j| }
a.set_row( 0, [ s(x1,x1), s(x1,x2), s(x1,y) ])
a.set_row( 1, [ s(x2,x1), s(x2,x2), s(x2,y) ])

a.display

a.left_eliminate!
a.display

b1 = a[0,2]
b2 = a[1,2]
b0 = y.avg - b1 * x1.avg - b2 * x2.avg

print b0,"\n"

b1d = b1 * Math::sqrt( s(x1,x1) / s(y,y))
print b1d,"\n"

b2d = b2 * Math::sqrt( s(x2,x2) / s(y,y))
print b2d,"\n"

