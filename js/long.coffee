print=console.log

print("print=console.log")
print("total=0")

n=10000
for i in [0..n]
    print "func_#{i} = (x) -> total +=x"

for i in [0..n]
    print "func_#{i}(#{i})"

print "print total"