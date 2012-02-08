sys = require "sys"

now = () ->
    nt = new Date()
    nt.getTime()

st = now()
sys.puts "start:#{st}"

t = 0
for i in [0..100000000]
    t += Math.random()

et = now()

sys.puts "ttt:#{et-st},#{t}"

