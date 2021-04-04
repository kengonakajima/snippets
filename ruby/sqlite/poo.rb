#

require "rubygems"
require "sqlite3"

db = SQLite3::Database.new("inv.db")

db.execute( "create table if not exists entries ( id varchar(80), doc_id varchar(16) )" )
begin
  db.execute( "create index idindex on entries(id)")
rescue
end


#db.execute( "insert into entries values ( 'zoooo#{Process.pid}', 'foo#{Process.pid}' ) " )

10000.times do |i|
  k = (rand()*65536).to_i
  db.execute( "insert into entries values ( 'id#{k}', 'val=#{Process.pid}' ) " )
end

STDERR.print "ins done\n"

db.execute( "select * from entries limit 10 " ) do |row|
  puts "row:  " + row.join(":")
end

db.close


