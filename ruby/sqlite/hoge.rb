#

require "rubygems"
require "sqlite3"

db = SQLite3::Database.new("data.db")

db.execute( "create table if not exists entries ( id integer primary key autoincrement, title varchar(100), body blob )" )


db.execute( "insert into entries values ( NULL, 'foo', 'hoooooooooooooooo' ) " )
db.execute( "insert into entries values ( NULL, ?, ? )",  "boo", "mooooooooooooooooooooo")
db.execute( "select * from entries" ) do |row|
  puts "row:  " + row.join(":")
end

print "---\n"
db.execute( "select * from sqlite_master") do |row|
  puts row.join(":")
end
  
db.close


