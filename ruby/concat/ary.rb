a = [1,2,3]
b = [5,6,7]

c = a + b
print c[5], "\n"

a = Dir.glob( "/etc/*")
b = Dir.glob( "/var/*")
c = a + b
print a.size, "\n"
print b.size, "\n"
print c.size, "\n" 
c.each do |x|
print x,"\n"
end
