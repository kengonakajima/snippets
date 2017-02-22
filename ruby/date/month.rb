require "date"

sd = Date.parse("2016-10-01")
ed = Date.parse("2017-02-07")

curd = sd
9999.times do
  msd = Date.new( curd.year, curd.month, 1 )
  med = (msd.next_month)-1
  print msd, " ", med, "\n"
  break if med > ed
  curd = msd.next_month
end

