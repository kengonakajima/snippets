a=[26.032, 27.098, 20.445, 27.581, 26.099, 20.399, 19.284, 27.523]
print "sum ", a.sum,"\n"
avg = a.sum / a.size
print "avg ", avg,"\n"
tot=a.map do |x| (x-avg)*(x-avg) end
print "stddev ", Math.sqrt(tot.sum/a.size), "\n"
