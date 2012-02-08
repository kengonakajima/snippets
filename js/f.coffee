sys = require "sys"

fff = (x) -> x + 2
ggg = (x) ->
    a={}
    a.p = x
    a.p

t = 0
for i in [0..100000000]
    t += ggg(i)
sys.puts "ttt:#{t}"

