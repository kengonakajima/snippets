h={}
n=10000
t = Time.now
1000000.times { |i|
    n += 1
    h[n]=i
}
e = Time.now
print (e - t), "\n"

t = Time.now
total=0
1000000.times {
total += h[n]
}

e = Time.now

print total, ":" , ( e - t ), "\n"


