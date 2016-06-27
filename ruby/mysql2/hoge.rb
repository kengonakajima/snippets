#
#
require "rubygems"
require "mysql2"

client = Mysql2::Client.new(:host => "localhost", :username => "root", :database => "test" )

print client

res = client.query("create table if not exists aho (id int) " )

print res

100.times do |i|
  client.query("insert into aho set id=#{i}" )
end

res = client.query("select * from aho where id < 10 limit 20")

res.each do |row|
  print row["id"],"\n"
end

#

st = client.prepare( "select * from aho where id < ? limit ?" )
res = st.execute(5,3)
res.each do |row|
  print "x",row["id"],"\n"
end


