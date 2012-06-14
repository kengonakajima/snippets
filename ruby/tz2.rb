require "webrick"

def todate(t)
  return sprintf( "%04d-%02d-%02d %02d:%02d:%02d", t.year,t.month,t.day, t.hour,t.min,t.sec )
end



print todate(Time.now),"\n"

t1 = Time.gm(2012,06,14,13,37,0, "+08:00")
print t1.to_s,"\n"
print todate(t1),"\n"

tp = Time.parse("2012-06-14T13:37:00+08:00")
print tp.to_s,"\n"
