require "rubygems"
require "mysql"


client = Mysql.connect( "localhost", "root", "", "test" )

10000.times do |i|
  dr = "drop table if exists poo_#{i};"
  client.query(dr)
#  cr = "create table poo_#{i} ( date datetime, user_id int, action char(16), data0 bigint, data1 char(64), data2 int, data3 int );" 
#  client.query(cr)
#  ins = "insert into poo_#{i} values ( now(), 11111, 'aho', 0, 'hoge', 1,2);"
#  client.query(ins)
#  print cr, "\n"
end


