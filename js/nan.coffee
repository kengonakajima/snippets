print = console.log


print "null*1:#{null*1}"
print "null*null:#{null*null}"

print "undef*1:#{undefined*1}"
print "undef*undef:#{undefined*undefined}"

a=undefined
b=undefined
print "a*b:#{a*b}"

a=1
print "a.b:#{a.b}"
#print "a.b:#{a.b.c}" ->

# a = hoge  -> hoge is not defined
if null
    print("null is true")
else
    print("null is false")

if undefined
    print("undef is true")
else
    print("undef is false")

if 0
    print("0 is true")

if 1
    print("1 is true")

if ""
    print("'' is true")

s0 = "aho"
s1 = "aho"
s2 = s0

if s0 == s1
    print("s0==s1")

if s0 == s2
    print("s0==s2")




# 0がfalseになるので、
#