s = "2012-02-14 13:32:16"

a,b,y,m,d,h,min,sec = string.find(s, "(%d+)-(%d+)-(%d+) (%d+):(%d+):(%d+)")

print("a:",a)
print("b:",b)
print("y:",y)
print("m:",m)
print("d:",d)
print("h:",h)
print("min:",min)
print("sec:",sec)
