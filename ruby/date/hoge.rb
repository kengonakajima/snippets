require "date"

sd = Date.parse("2017-02-01")
ed = Date.parse("2017-02-07")

unit_days = 3
diff_days = (ed.mjd - sd.mjd + 1 ) # 終端の当日を含む

mod_unit = if (diff_days % unit_days) > 0 then 1 else 0 end
unit_num = (diff_days / unit_days).to_i + mod_unit

units=[]
unit_num.times do |i|
  usd = sd+(i*unit_days)
  ued = sd+(i*unit_days)+unit_days-1
  if ued > ed then ued = ed end
  units.push( [usd,ued] )
end


print diff_days, " ", mod_unit, " ", unit_num, " ", units, "\n"