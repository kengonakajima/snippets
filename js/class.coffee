class Char
    constructor: (a,b,c) ->
        @a =a
        @b = b
        @c = c
    total: () ->
        @a + @b + @c


n=10000

chars=[]

for i in [0..n]
    chars.push( new Char(i,i,i) )

o = chars[n/2]

total = 0
for i in [0..100000000]
    total += o.total()

console.log total
