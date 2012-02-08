# calc the first second of the day 
a = Time.now
b = a - ( a.hour * 60 * 60 ) - ( a.min * 60 ) - ( a.sec )
print b.to_s, "\n"
