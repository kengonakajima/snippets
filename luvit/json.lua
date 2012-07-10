JSON=require("json")

a = { [8] = 100, [110] = 200 }

s=JSON.stringify(a)
p(s)

t=JSON.parse(s)
p( t[8] )
p( t[110] )

assert( t[8] == 100 )

