require "rubygems"
require "mysql"

my = Mysql::new( "192.168.1.121", "ringo", "", "vcx_dev")

#res = my.query("select * from players")
#res.each do |row|
#  col1 = row[0]
#  col2 = row[1]
#  print col1, ",", col2, "\n"
#end

my.query( "drop table if exists hoge" )
my.query( "create table hoge ( id int )" )

n = 20
ary = []
n.times {
  t = Thread.new do
    (1000 * 1000 * 10 / n ).times { |i|
      STDERR.print "." if ( i % (100) ) == 0 
      my.query( "insert into hoge set id = #{i}" )
    }
  end
  ary.push(t)
}

ary.each do |t|
  t.join
end

