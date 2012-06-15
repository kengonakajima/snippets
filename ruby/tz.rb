require "webrick"

def todate(t)
  return sprintf( "%04d-%02d-%02d %02d:%02d:%02d", t.year,t.month,t.day, t.hour,t.min,t.sec )
end

orig = "2012-06-13T19:04:10-07:00"
print "step 1:", orig, "\n"

mysql = "2012-06-13T19:04:10"
print "step 2:",mysql,"\n"

parsed = Time.parse(mysql)
print "step 3:", parsed,"\n"


print "step 4:", (parsed.to_i - Time.parse(orig).to_i)/3600, "\n"




# 1. ghから -7時間のiso8601が来る
# 2. それをmysqlに入れるとtzが削れて入る。　これでto_iが7時間分減る。
# 3. それをTime.parseすると, tzがないので日本のになり、 +09:00　とパースするのでさらに9時間分減る。
# 4. to_iで比較すると　. 合計16時間引かれる。



print "==============\n"


# mysqlのdatetimeには、タイムゾーンの値は入らない。　そこでこれを直すには、mysqlにto_iしてから入れる。


orig = "2012-06-13T19:04:10-07:00"
print "step 1:", orig, "\n"

mysql = Time.parse(orig).to_i
print "step 2:",mysql,"\n"

parsed = Time.at(mysql)
print "step 3:", parsed,"\n"


print "step 4:", (parsed.to_i - Time.parse(orig).to_i)/3600, "\n"


#
# 全部bigintにしたら解決できるが、human readableでなくなる。
# 地道に解決すると、
# 2. mysqlに入れるときに7時間分足す
# 3. mysqlから出すときに Time.parseを  UTCをつけて実行する

print "==============\n"

orig = "2012-06-13T19:04:10-07:00" # 北米陣が感じている時間
print "step 1:", orig, "\n"

before_mysql = Time.iso8601(orig)  # そのままTimeにする

print "AAAAAAAAAA:", before_mysql.hour, ",", before_mysql.gmtoff, ",", before_mysql.utc_offset, "\n"  # hourを出すと、日本人が感じてる時間を出力してしまう。
print before_mysql.methods.sort.join("\n")

mysql = todate(before_mysql)
print "step 2:",mysql,"\n"

parsed = Time.parse(mysql+" UTC")
print( "xx:", parsed.gmtoff )
print "step 3:", parsed,"\n"

dt = (parsed.to_i - Time.parse(orig).to_i)/3600
print "step 4:", dt, "\n"
raise "differ" if dt != 0 



exit 0
