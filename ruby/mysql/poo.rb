require "mysql"
require "date"

my = Mysql.connect("localhost", "root","", "test")

my.query( "drop table if exists hogehoge" )

my.query( "create table if not exists hogehoge ( id int not null primary key auto_increment, name char(50), createdAt datetime )" )

my.query( "insert into hogehoge set name='aa', createdAt=now() " )
my.query( "insert into hogehoge set name='aa', createdAt=now() " )
my.query( "insert into hogehoge set name='aa', createdAt=now() " )

print my.insert_id, "\n"

res = my.query( "select id,name,createdAt from hogehoge" )

print res.methods,"\n"

h = res.fetch_hash
print h, "\n"




res = my.query( "select id,name,createdAt from hogehoge" )


res.each_hash do |h|
  print h, "\n"  
end


res = my.real_query( "select id,name,createdAt from hogehoge" )

res.each_hash do |h|
  print h, "\n"
end
  