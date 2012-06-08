require "./wrap"

my = MysqlWrapper.new("localhost", "root","", "test")

my.query( "drop table if exists hogehoge" )

my.query( "create table if not exists hogehoge ( id int not null primary key auto_increment, name char(50), createdAt datetime )" )

my.query( "insert into hogehoge set name='aa', createdAt=now() " )
my.query( "insert into hogehoge set name='aa', createdAt=now() " )
my.query( "insert into hogehoge set name='aa', createdAt=now() " )


res = my.query( "select id,name,createdAt from hogehoge" )

res.each do |ent|
  print("e:",ent["id"], ",", ent["name"], ",", ent["createdAt"].to_i, "\n" )
end


