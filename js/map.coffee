a = {}

for i in [0..10000000]
    a[i] = i*10

tot = 0
for i in [0..10000000]
    tot += a[i]

